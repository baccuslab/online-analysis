/* settings.cpp
 * Implemenation file for various settings and the Settings class.
 * (C) 2015 Benjamin Naecker bnaecker@stanford.edu.
 */

#include "settings.h"

Settings::Settings() {
	//settings = new QSettings();
}

Settings::~Settings() {
}

float Settings::getDisplayRange() {
	return this->settings.value("display-range").toFloat();
}

float Settings::getDisplayScale() {
	return this->settings.value("display-scale").toFloat();
}

void Settings::setDisplayScale(float scale) {
	this->settings.setValue("display-scale", QVariant(scale));
}

QList<float> Settings::getDisplayScales() {
	return DISPLAY_SCALES;
}

unsigned int Settings::getRefreshInterval() {
	return this->settings.value("refresh-interval").toFloat();
}

void Settings::setRefreshInterval(unsigned int interval) {
	float i = qMin(qMax(interval, MIN_REFRESH_INTERVAL), MAX_REFRESH_INTERVAL);
	this->settings.setValue("refresh-interval", QVariant(i));
}

QString Settings::getPlotColorString() {
	return this->settings.value("plot-color-label").toString();
}

QColor Settings::getPlotColor() {
	return this->settings.value("plot-color").value<QColor>();
}

void Settings::setPlotColor(QString color) {
	this->settings.setValue("plot-color-label", QVariant(color));
	this->settings.setValue("plot-color", QVariant(PLOT_COLOR_MAP[color].first));
	this->settings.setValue("plot-pen", QVariant(PLOT_COLOR_MAP[color].second));
}

QPen Settings::getPlotPen() {
	return this->settings.value("plot-pen").value<QPen>();
}

QStringList Settings::getPlotColorStrings() {
	return PLOT_COLOR_STRINGS;
}

QList<QPair<int, int> > Settings::getChannelView() {
	return CHANNEL_VIEW_MAP[this->getChannelViewString()];
}

QString Settings::getChannelViewString() {
	return this->settings.value("channel-view-string").toString();
}

void Settings::setChannelView(QString s) {
	this->settings.setValue("channel-view-string", QVariant(s));
}

QString Settings::getSaveDir() {
	return this->settings.value("save-dir").toString();
}

void Settings::setSaveDir(QString s) {
	this->settings.setValue("save-dir", QVariant(s));
}

QString Settings::getSaveFilename() {
	return this->settings.value("save-filename").toString();
}

void Settings::setSaveFilename(QString s) {
	this->settings.setValue("save-filename", QVariant(s));
}

unsigned int Settings::getExperimentLength() {
	return this->settings.value("experiment-length").toUInt();
}

void Settings::setExperimentLength(unsigned int len) {
	this->settings.setValue("experiment-length", QVariant(len));
}

bool Settings::getAutoscale() {
	return this->settings.value("autoscale").toBool();
}

void Settings::setAutoscale(bool on) {
	this->settings.setValue("autoscale", QVariant(on));
}

unsigned int Settings::getOnlineAnalysisLength() {
	return this->settings.value("online-analysis-length").toUInt();
}

void Settings::setOnlineAnalysisLength(unsigned int len) {
	unsigned int val = qMax(qMin(len, ONLINE_ANALYSIS_MAX_LENGTH), 
			ONLINE_ANALYSIS_MIN_LENGTH);
	this->settings.setValue("online-analysis-length", QVariant(val));
}

unsigned int Settings::getJump() {
	return this->settings.value("jump").toUInt();
}

void Settings::setJump(unsigned int jump) {
	unsigned int val = qMax(qMin(jump, JUMP_MAX), JUMP_MIN);
	this->settings.setValue("jump", QVariant(val));
}

int Settings::getNumRows() {
	return this->settings.value("num-rows").toInt();
}

int Settings::getNumCols() {
	return this->settings.value("num-cols").toInt();
}

void Settings::setNumRows(int rows) {
	this->settings.setValue("num-rows", QVariant(rows));
}

void Settings::setNumCols(int cols) {
	this->settings.setValue("num-cols", QVariant(cols));
}

bool Settings::getAutoMean() {
	return this->settings.value("automean").toBool();
}

void Settings::setAutoMean(bool b) {
	this->settings.setValue("automean", QVariant(b));
}
