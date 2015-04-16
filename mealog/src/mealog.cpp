/* mealog.cpp
 * This file implements the mealog application, a simple GUI program
 * to stream data from the NI-DAQ server running on the Windows machine,
 * and log the data to the localhost in HDF5 format.
 *
 * This application can be used to set parameters, start and stop 
 * recordings.
 *
 * (C) 2015 Benjamin Naecker bnaecker@stanford.edu
 */

#include <string>

#include <QApplication>
#include <QVariant>
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>
#include <QTime>
#include <QDate>

#include "mealog.h"

MealogWindow::MealogWindow(QWidget *parent) : QMainWindow(parent) {
	initGui();
	initServer();
	initSignals();
}

MealogWindow::~MealogWindow() {
}

void MealogWindow::initGui(void) {
	this->setWindowTitle("Mealog");
	this->setGeometry(10, 10, 300, 200);
	mainLayout = new QGridLayout();

	nidaqGroup = new QGroupBox("NI-DAQ");
	nidaqLayout = new QGridLayout();
	connectButton = new QPushButton("Connect");
	connectButton->setToolTip("Connect to NI-DAQ server to initialize the recording");
	nidaqStatusLabel = new QLabel("Status:");
	nidaqStatusLabel->setAlignment(Qt::AlignRight);
	nidaqStatus = new QLabel("Not connected");
	nidaqStatus->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	nidaqStatus->setToolTip("Status of connection with NIDAQ");
	nidaqLayout->addWidget(connectButton, 0, 0);
	nidaqLayout->addWidget(nidaqStatusLabel, 0, 1);
	nidaqLayout->addWidget(nidaqStatus, 0, 2, 1, 4);
	nidaqGroup->setLayout(nidaqLayout);

	ctrlGroup = new QGroupBox("Controls");
	ctrlLayout = new QGridLayout();
	initRecordingButton = new QPushButton("Initialize");
	initRecordingButton->setToolTip("Initialize a recording with the given parameters");
	startMeaviewButton = new QPushButton("Start Meaview");
	startMeaviewButton->setToolTip("Start Meaview application to visualize recording");
	if (checkMeaviewRunning())
		startMeaviewButton->setEnabled(false);
	startButton = new QPushButton("Start");
	startButton->setToolTip("Start recording");
	startButton->setEnabled(false);
	quitButton = new QPushButton("Quit");
	quitButton->setToolTip("Quit Mealog");
	ctrlLayout->addWidget(initRecordingButton, 0, 1);
	ctrlLayout->addWidget(startButton, 0, 2);
	ctrlLayout->addWidget(quitButton, 0, 3);
	ctrlGroup->setLayout(ctrlLayout);

	paramGroup = new QGroupBox("Recording parameters");
	paramLayout = new QGridLayout();

	timeLabel = new QLabel("Time:");
	timeLine = new QLineEdit(QString::number(DEFAULT_EXPERIMENT_LENGTH));
	timeLine->setToolTip("Set duration of the recording");
	timeValidator = new QIntValidator(1, MAX_EXPERIMENT_LENGTH);
	timeLine->setValidator(timeValidator);

	fileLabel = new QLabel("Data file:");
	fileLine = new QLineEdit(DEFAULT_SAVE_FILE.fileName(), this);
	fileLine->setToolTip("Name of file to which data is written");
	fileValidator = new QRegExpValidator(QRegExp("(\\w+[-_]*)+"));
	fileLine->setValidator(fileValidator);

	savedirLabel = new QLabel("Save dir:");
	savedirLine = new QLineEdit(DEFAULT_SAVE_DIR.absolutePath());
	savedirLine->setToolTip("Directory of current recording data file");
	savedirLine->setReadOnly(true);
	chooseDirButton = new QPushButton("Choose");
	chooseDirButton->setToolTip("Choose save directory");

	adcRangeBox = new QComboBox(this);
	for (auto &each : ADC_RANGES)
		adcRangeBox->addItem(QString::number(each), QVariant(each));
	adcRangeLabel = new QLabel("ADC range:");
	adcRangeBox->setToolTip("Set the voltage range of the NI-DAQ card");
	adcRangeBox->setCurrentIndex(ADC_RANGES.indexOf(DEFAULT_ADC_RANGE));

	triggerBox = new QComboBox(this);
	triggerBox->addItems(TRIGGERS);
	triggerLabel = new QLabel("Trigger:");
	triggerBox->setToolTip("Set the triggering mechanism for starting the experiment");

	paramLayout->addWidget(timeLabel, 1, 0);
	paramLayout->addWidget(timeLine, 1, 1);
	paramLayout->addWidget(fileLabel, 1, 2);
	paramLayout->addWidget(fileLine, 1, 3, 1, 2);
	paramLayout->addWidget(savedirLabel, 2, 0);
	paramLayout->addWidget(savedirLine, 2, 1, 1, 3);
	paramLayout->addWidget(chooseDirButton, 2, 4);
	paramLayout->addWidget(adcRangeLabel, 3, 0);
	paramLayout->addWidget(adcRangeBox, 3, 1);
	paramLayout->addWidget(triggerLabel, 3, 2);
	paramLayout->addWidget(triggerBox, 3, 3);
	paramGroup->setLayout(paramLayout);

	mainLayout->addWidget(nidaqGroup, 0, 0);
	mainLayout->addWidget(ctrlGroup, 1, 0);
	mainLayout->addWidget(paramGroup, 2, 0);

	statusBar = new QStatusBar(this);
	statusBar->showMessage("Ready");
	this->setStatusBar(statusBar);
	this->setCentralWidget(new QWidget(this));
	this->centralWidget()->setLayout(mainLayout);
}

void MealogWindow::initServer(void) {
	server = new QTcpServer(this);
	server->listen(QHostAddress::LocalHost, IPC_PORT);
}

void MealogWindow::acceptClients(void) {
	QTcpSocket *socket = server->nextPendingConnection();
	connect(socket, SIGNAL(bytesWritten(quint64)), 
			this, SLOT(respondToClient(quint64)));
}

void MealogWindow::respondToClient(quint64 nbytes) {
	QTcpSocket *s = dynamic_cast<QTcpSocket *>(QObject::sender());
	mearec::RecordingStatusRequest req;
	bool ok = req.ParseFromString(s->read(nbytes).toStdString());
}

QFile *MealogWindow::getFullFilename(void) {
	QDir d(savedirLine->text());
	QFile *path = new QFile(QDir::cleanPath(d.absolutePath() + "/" + fileLine->text()));
	return path;
}

bool MealogWindow::removeOldRecording(QFile &path) {
	if (!path.remove()) {
		QMessageBox box(this);
		box.setText("Error");
		box.setInformativeText(
				"Could not overwrite the requested file. \
				Remove manually and try again.");
		box.setStandardButtons(QMessageBox::Ok);
		box.setDefaultButton(QMessageBox::Ok);
		box.exec();
		return false;
	}
	return true;
}

void MealogWindow::setRecordingParameters(void) {
	/* Set length and sample information */
	recording->setLength(timeLine->text().toDouble());
	recording->setLive(true);
	recording->setLastValidSample(0);

	/* Use defaults */
	recording->setFileType(recording->type());
	recording->setFileVersion(recording->version());
	recording->setSampleRate(recording->sampleRate());

	recording->setNumSamples(recording->length() * recording->sampleRate());
	double adcRange = ADC_RANGES.at(adcRangeBox->currentIndex());
	recording->setOffset(adcRange);
	recording->setGain((adcRange * 2) / (1 << 16));

	/* Date and time */
	QString tm(QTime::currentTime().toString("h:mm:ss AP"));
	QString dt(QDate::currentDate().toString("ddd, MMM dd, yyyy"));
	recording->setTime(tm.toStdString());
	recording->setDate(dt.toStdString());
}

void MealogWindow::setParameterSelectionsEnabled(bool enabled) {
	adcRangeBox->setEnabled(enabled);
	triggerBox->setEnabled(enabled);
	timeLine->setEnabled(enabled);
	fileLine->setEnabled(enabled);
	savedirLine->setEnabled(enabled);
	chooseDirButton->setEnabled(enabled);
}

void MealogWindow::initRecording(void) {
	statusBar->showMessage("Initializing recording");
	QFile *path = getFullFilename();
	if (path->exists()) {
		if (!confirmFileOverwrite(*path))
			return;
		if (!removeOldRecording(*path))
			return;
	}

	/* Make recording and set parameters from choices */
	recording = new H5Recording(path->fileName().toStdString());
	setRecordingParameters();

	/* Disable parameter selections */
	setParameterSelectionsEnabled(false);

	/* Change init button to "un-init" */
	initRecordingButton->setText("Reset parameters");
	initRecordingButton->setToolTip("Destroy the current recording and reset parameters");
	disconnect(initRecordingButton, SIGNAL(clicked()), 
			this, SLOT(initRecording()));
	connect(initRecordingButton, SIGNAL(clicked()),
			this, SLOT(deInitRecording()));
	if (nidaqConnected)
		startButton->setEnabled(true);
	recordingInitialized = true;
	delete path;
	statusBar->showMessage("Ready");
}

void MealogWindow::deInitRecording(void) {
	statusBar->showMessage("Reseting recording");
	/* Close the recording file */
	delete recording;
	recording = nullptr;

	/* Remove the file */
	if (!removeOldRecording(*getFullFilename()))
		return;

	/* Re-enabled parameter selection */
	setParameterSelectionsEnabled(true);

	/* Change buttons */
	startButton->setEnabled(false);
	initRecordingButton->setText("Initialize");
	initRecordingButton->setToolTip("Initialize a recording with the given parameters");
	disconnect(initRecordingButton, SIGNAL(clicked()), 
			this, SLOT(deInitRecording()));
	connect(initRecordingButton, SIGNAL(clicked()),
			this, SLOT(initRecording()));
	recordingInitialized = false;
	statusBar->showMessage("Ready");
}

bool MealogWindow::confirmFileOverwrite(const QFile &path) {
	QMessageBox box(this);
	box.setWindowTitle("File exists"); // ignore on OSX
	box.setText("The selected file already exists.");
	box.setInformativeText("The file \"" + 
			path.fileName() + "\" already exists. Overwrite?");
	box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	box.setDefaultButton(QMessageBox::Cancel);
	return box.exec();
}

void MealogWindow::initSignals(void) {
	connect(this->quitButton, SIGNAL(clicked()), 
			qApp, SLOT(quit()));
	connect(this->startMeaviewButton, SIGNAL(clicked()), 
			this, SLOT(startMeaview()));
	connect(this->server, SIGNAL(newConnection()), 
			this, SLOT(acceptClients()));
	connect(this->initRecordingButton, SIGNAL(clicked()), 
			this, SLOT(initRecording()));
}

bool MealogWindow::checkMeaviewRunning(void) {
	QProcess pgrep;
	pgrep.setProgram("pgrep");
	QStringList args = {"meaview"};
	pgrep.setArguments(args);
	pgrep.start();
	pgrep.waitForFinished();
	return (pgrep.readAllStandardOutput().length() > 0);
}

void MealogWindow::startMeaview(void) {
	if (checkMeaviewRunning())
		return;
	QProcess m;
	m.startDetached("/Users/bnaecker/FileCabinet/stanford/baccuslab/mearec/meaview/meaview.app/Contents/MacOS/meaview");
	m.waitForStarted();
}
