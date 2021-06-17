/*
    Qalculate (QT UI)

    Copyright (C) 2021  Hanna Knutsson (hanna.knutsson@protonmail.com)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
*/

#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QToolTip>
#include <QComboBox>
#include <QLabel>
#include <QDebug>

#include "qalculateqtsettings.h"
#include "preferencesdialog.h"

#define BOX(t, x, f) box = new QCheckBox(t, this); l->addWidget(box); box->setChecked(x); connect(box, SIGNAL(toggled(bool)), this, SLOT(f));
#define BOX_G(t, x, f) box = new QCheckBox(t, this); l2->addWidget(box, r, 0, 1, 2); box->setChecked(x); connect(box, SIGNAL(toggled(bool)), this, SLOT(f));

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent) {
	QVBoxLayout *topbox = new QVBoxLayout(this);
	QTabWidget *tabs = new QTabWidget(this);
	tabs->setUsesScrollButtons(false);
	topbox->addWidget(tabs);
	QWidget *w1 = new QWidget(this);
	QWidget *w2 = new QWidget(this);
	QWidget *w3 = new QWidget(this);
	QWidget *w4 = new QWidget(this);
	tabs->addTab(w1, tr("General"));
	tabs->addTab(w2, tr("Numbers && Operators"));
	tabs->addTab(w3, tr("Units && Currencies"));
	tabs->addTab(w4, tr("Fonts && Colors"));
	QCheckBox *box; QVBoxLayout *l; QGridLayout *l2; QComboBox *combo;
	l = new QVBoxLayout(w1);
	BOX(tr("Ignore system language (requires restart)"), settings->ignore_locale, ignoreLocaleToggled(bool));
	BOX(tr("Keep above other windows"), settings->always_on_top, keepAboveToggled(bool));
	BOX(tr("Display expression status"), settings->display_expression_status, expressionStatusToggled(bool));
	int r = 0;
	l2 = new QGridLayout();
	l2->addWidget(new QLabel(tr("Window title:"), this), r, 0);
	combo = new QComboBox(this);
	combo->addItem(tr("Application name"), TITLE_APP);
	combo->addItem(tr("Result"), TITLE_RESULT);
	combo->addItem(tr("Application name + result"), TITLE_APP_RESULT);
	combo->setCurrentIndex(combo->findData(settings->title_type));
	connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(titleChanged(int)));
	l2->addWidget(combo, r, 1); r++;
	l2->addWidget(new QLabel(tr("Parsing mode:"), this), r, 0);
	combo = new QComboBox(this);
	combo->addItem(tr("Adaptive"), PARSING_MODE_ADAPTIVE);
	combo->addItem(tr("Conventional"), PARSING_MODE_CONVENTIONAL);
	combo->addItem(tr("Implicit multiplication first"), PARSING_MODE_IMPLICIT_MULTIPLICATION_FIRST);
	combo->addItem(tr("Chain"), PARSING_MODE_CHAIN);
	combo->addItem(tr("RPN"), PARSING_MODE_RPN);
	combo->setCurrentIndex(combo->findData(settings->evalops.parse_options.parsing_mode));
	connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(parsingModeChanged(int)));
	l2->addWidget(combo, r, 1); r++;
	BOX_G(tr("Read precision"), settings->evalops.parse_options.read_precision != DONT_READ_PRECISION, readPrecisionToggled(bool)); r++;
	BOX_G(tr("Limit implicit multiplication"), settings->evalops.parse_options.limit_implicit_multiplication, limitImplicitToggled(bool)); r++;
	l2->addWidget(new QLabel(tr("Interval calculation:"), this), r, 0);
	combo = new QComboBox(this);
	combo->addItem(tr("Variance formula"), INTERVAL_CALCULATION_VARIANCE_FORMULA);
	combo->addItem(tr("Interval arithmetic"), INTERVAL_CALCULATION_INTERVAL_ARITHMETIC);
	combo->setCurrentIndex(combo->findData(settings->evalops.interval_calculation));
	connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(intervalCalculationChanged(int)));
	l2->addWidget(combo, r, 1); r++;
	l->addLayout(l2);
	l->addStretch(1);
	l = new QVBoxLayout(w2);
	BOX(tr("Binary two's complement representation"), settings->printops.twos_complement, binTwosToggled(bool));
	BOX(tr("Hexadecimal two's complement representation"), settings->printops.hexadecimal_twos_complement, hexTwosToggled(bool));
	BOX(tr("Use lower case letters in non-decimal numbers"), settings->printops.lower_case_numbers, lowerCaseToggled(bool));
	BOX(tr("Spell out logical operators"), settings->printops.spell_out_logical_operators, spellOutToggled(bool));
	BOX(tr("Use E-notation instead of 10^x"), settings->printops.lower_case_e, eToggled(bool));
	BOX(tr("Use 'j' as imaginary unit"), CALCULATOR->getVariableById(VARIABLE_ID_I)->hasName("j") > 0, imaginaryJToggled(bool));
	BOX(tr("Use comma as decimal separator"), CALCULATOR->getDecimalPoint() == COMMA, decimalCommaToggled(bool));
	BOX(tr("Ignore comma in numbers"), settings->evalops.parse_options.comma_as_separator, ignoreCommaToggled(bool)); ignoreCommaBox = box;
	BOX(tr("Ignore dots in numbers"), settings->evalops.parse_options.dot_as_separator, ignoreDotToggled(bool)); ignoreDotBox = box;
	BOX(tr("Round halfway numbers to even"), settings->printops.round_halfway_to_even, roundEvenToggled(bool));
	if(CALCULATOR->getDecimalPoint() == COMMA) ignoreCommaBox->hide();
	if(CALCULATOR->getDecimalPoint() == DOT) ignoreDotBox->hide();
	l2 = new QGridLayout();
	r = 0;
	l2->addWidget(new QLabel(tr("Digit grouping:"), this), r, 0);
	combo = new QComboBox(this);
	combo->addItem(tr("None"), DIGIT_GROUPING_NONE);
	combo->addItem(tr("Standard"), DIGIT_GROUPING_STANDARD);
	combo->addItem(tr("Local"), DIGIT_GROUPING_LOCALE);
	combo->setCurrentIndex(combo->findData(settings->printops.digit_grouping));
	connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(groupingChanged(int)));
	l2->addWidget(combo, r, 1); r++;
	l2->addWidget(new QLabel(tr("Interval display:"), this), r, 0);
	combo = new QComboBox(this);
	combo->addItem(tr("Adaptive"), -1);
	combo->addItem(tr("Significant digits"), INTERVAL_DISPLAY_SIGNIFICANT_DIGITS);
	combo->addItem(tr("Interval"), INTERVAL_DISPLAY_INTERVAL);
	combo->addItem(tr("Plus/minus"), INTERVAL_DISPLAY_PLUSMINUS);
	combo->addItem(tr("Midpoint"), INTERVAL_DISPLAY_MIDPOINT);
	combo->addItem(tr("Lower"), INTERVAL_DISPLAY_LOWER);
	combo->addItem(tr("Upper"), INTERVAL_DISPLAY_UPPER);
	if(settings->adaptive_interval_display) combo->setCurrentIndex(0);
	else combo->setCurrentIndex(combo->findData(settings->evalops.interval_calculation));
	connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(intervalDisplayChanged(int)));
	l2->addWidget(combo, r, 1); r++;
	l2->addWidget(new QLabel(tr("Complex number form:"), this), r, 0);
	combo = new QComboBox(this);
	combo->addItem(tr("Rectangular"), COMPLEX_NUMBER_FORM_RECTANGULAR);
	combo->addItem(tr("Exponential"), COMPLEX_NUMBER_FORM_EXPONENTIAL);
	combo->addItem(tr("Polar"), COMPLEX_NUMBER_FORM_POLAR);
	combo->addItem(tr("Angle/phasor"), COMPLEX_NUMBER_FORM_CIS);
	combo->setCurrentIndex(combo->findData(settings->evalops.complex_number_form));
	connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(complexFormChanged(int)));
	l2->addWidget(combo, r, 1); r++;
	l->addLayout(l2);
	l->addStretch(1);
	l2 = new QGridLayout(w3);
	r = 0;
	BOX_G(tr("Use binary prefixes for information units"), CALCULATOR->usesBinaryPrefixes() > 0, binaryPrefixesToggled(bool)); r++;
	l2->addWidget(new QLabel(tr("Automatic unit conversion:"), this), r, 0);
	combo = new QComboBox(this);
	combo->addItem(tr("No conversion"), POST_CONVERSION_NONE);
	combo->addItem(tr("Base units"), POST_CONVERSION_BASE);
	combo->addItem(tr("Optimal units"), POST_CONVERSION_OPTIMAL);
	combo->addItem(tr("Optimal SI units"), POST_CONVERSION_OPTIMAL_SI);
	combo->setCurrentIndex(combo->findData(settings->evalops.auto_post_conversion));
	l2->addWidget(combo, r, 1); r++;
	connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(conversionChanged(int)));
	BOX_G(tr("Convert to mixed units"), settings->evalops.mixed_units_conversion != MIXED_UNITS_CONVERSION_NONE, mixedUnitsToggled(bool)); r++;
	l2->addWidget(new QLabel(tr("Automatic unit prefixes:"), this), r, 0);
	combo = new QComboBox(this);
	combo->addItem(tr("Default"));
	combo->addItem(tr("No prefixes"));
	combo->addItem(tr("Prefixes for some units"));
	combo->addItem(tr("Prefixes also for currencies"));
	combo->addItem(tr("Prefixes for all units"));
	if(settings->prefixes_default) combo->setCurrentIndex(0);
	else if(!settings->printops.use_unit_prefixes) combo->setCurrentIndex(1);
	else if(settings->printops.use_prefixes_for_all_units) combo->setCurrentIndex(4);
	else if(settings->printops.use_prefixes_for_currencies) combo->setCurrentIndex(3);
	else combo->setCurrentIndex(2);
	l2->addWidget(combo, r, 1); r++;
	connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(prefixesChanged(int)));
	BOX_G(tr("Enable all SI-prefixes"), settings->printops.use_all_prefixes, allPrefixesToggled(bool)); r++;
	BOX_G(tr("Enable denominator prefixes"), settings->printops.use_denominator_prefix, denominatorPrefixToggled(bool)); r++;
	BOX_G(tr("Enable units in physical constants"), CALCULATOR->variableUnitsEnabled(), variableUnitsToggled(bool)); r++;
	l2->addWidget(new QLabel(tr("Temperature calculation:"), this), r, 0);
	combo = new QComboBox(this);
	combo->addItem(tr("Absolute"), TEMPERATURE_CALCULATION_ABSOLUTE);
	combo->addItem(tr("Relative"), TEMPERATURE_CALCULATION_RELATIVE);
	combo->addItem(tr("Hybrid"), TEMPERATURE_CALCULATION_HYBRID);
	combo->setCurrentIndex(combo->findData(CALCULATOR->getTemperatureCalculationMode()));
	l2->addWidget(combo, r, 1); r++;
	connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(temperatureCalculationChanged(int)));
	box = new QCheckBox(tr("Exchange rates updates:"), this); box->setChecked(settings->auto_update_exchange_rates > 0); connect(box, SIGNAL(toggled(bool)), this, SLOT(exratesToggled(bool))); l2->addWidget(box, r, 0);
	QSpinBox *spin = new QSpinBox(this); spin->setRange(1, 100); spin->setSuffix(" " + tr("days")); spin->setValue(settings->auto_update_exchange_rates <= 0 ? 7 : settings->auto_update_exchange_rates); spin->setEnabled(settings->auto_update_exchange_rates > 0); connect(spin, SIGNAL(valueChanged(int)), this, SLOT(exratesChanged(int))); l2->addWidget(spin, r, 1); exratesSpin = spin; r++;
	l2->setRowStretch(r, 1);
	l = new QVBoxLayout(w4);
	BOX(tr("Colorize result"), settings->colorize_result, colorizeToggled(bool));
	l->addStretch(1);
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
	topbox->addWidget(buttonBox);
	connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(reject()));
	if(settings->always_on_top) setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
}
PreferencesDialog::~PreferencesDialog() {}

void PreferencesDialog::ignoreLocaleToggled(bool b) {
	settings->ignore_locale = b;
}
void PreferencesDialog::keepAboveToggled(bool b) {
	settings->always_on_top = b;
	if(settings->always_on_top) setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	else setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
	show();
	emit alwaysOnTopChanged();
}
void PreferencesDialog::expressionStatusToggled(bool b) {
	settings->display_expression_status = b;
	if(!b) QToolTip::hideText();
}
void PreferencesDialog::binTwosToggled(bool b) {
	settings->printops.twos_complement = b;
	emit resultDisplayUpdated();
}
void PreferencesDialog::hexTwosToggled(bool b) {
	settings->printops.hexadecimal_twos_complement = b;
	emit resultDisplayUpdated();
}
void PreferencesDialog::lowerCaseToggled(bool b) {
	settings->printops.lower_case_numbers = b;
	emit resultDisplayUpdated();
}
void PreferencesDialog::spellOutToggled(bool b) {
	settings->printops.spell_out_logical_operators = b;
	emit resultDisplayUpdated();
}
void PreferencesDialog::eToggled(bool b) {
	settings->printops.lower_case_e = b;
	emit resultDisplayUpdated();
}
void PreferencesDialog::imaginaryJToggled(bool b) {
	Variable *v_i = CALCULATOR->getVariableById(VARIABLE_ID_I);
	if(!v_i || (b != (CALCULATOR->v_i->hasName("j") > 0))) return;
	if(b) {
		ExpressionName ename = v_i->getName(1);
		ename.name = "j";
		ename.reference = false;
		v_i->addName(ename, 1, true);
		v_i->setChanged(false);
	} else {
		v_i->clearNonReferenceNames();
		v_i->setChanged(false);
	}
	emit expressionFormatUpdated(false);
}
void PreferencesDialog::decimalCommaToggled(bool b) {
	settings->decimal_comma = b;
	if(b) {
		CALCULATOR->useDecimalComma();
		ignoreCommaBox->hide();
		ignoreDotBox->show();
	} else {
		CALCULATOR->useDecimalPoint(settings->evalops.parse_options.comma_as_separator);
		ignoreCommaBox->show();
		ignoreDotBox->hide();
	}
	settings->dot_question_asked = true;
	emit expressionFormatUpdated(false);
	emit resultDisplayUpdated();
	emit symbolsUpdated();
}
void PreferencesDialog::ignoreCommaToggled(bool b) {
	settings->evalops.parse_options.dot_as_separator = b;
	settings->dot_question_asked = true;
	emit expressionFormatUpdated(false);
}
void PreferencesDialog::ignoreDotToggled(bool b) {
	settings->evalops.parse_options.comma_as_separator = b;
	CALCULATOR->useDecimalPoint(settings->evalops.parse_options.comma_as_separator);
	settings->dot_question_asked = true;
	emit expressionFormatUpdated(false);
	emit symbolsUpdated();
}
void PreferencesDialog::colorizeToggled(bool b) {
	settings->colorize_result = b;
	emit resultDisplayUpdated();
}
void PreferencesDialog::parsingModeChanged(int i) {
	settings->evalops.parse_options.parsing_mode = (ParsingMode) qobject_cast<QComboBox*>(sender())->itemData(i).toInt();
	emit expressionFormatUpdated(false);
}
void PreferencesDialog::temperatureCalculationChanged(int i) {
	CALCULATOR->setTemperatureCalculationMode((TemperatureCalculationMode) qobject_cast<QComboBox*>(sender())->itemData(i).toInt());
	emit expressionCalculationUpdated(0);
}
void PreferencesDialog::exratesToggled(bool b) {
	exratesSpin->setEnabled(b);
	settings->auto_update_exchange_rates = b ? exratesSpin->value() : 0;
}
void PreferencesDialog::exratesChanged(int i) {
	settings->auto_update_exchange_rates = i;
}
void PreferencesDialog::binaryPrefixesToggled(bool b) {
	CALCULATOR->useBinaryPrefixes(b ? 1 : 0);
	emit resultFormatUpdated();
}
void PreferencesDialog::readPrecisionToggled(bool b) {
	if(b) settings->evalops.parse_options.read_precision = READ_PRECISION_WHEN_DECIMALS;
	else settings->evalops.parse_options.read_precision = DONT_READ_PRECISION;
	emit expressionFormatUpdated(true);
}
void PreferencesDialog::intervalCalculationChanged(int i) {
	settings->evalops.interval_calculation = (IntervalCalculation) qobject_cast<QComboBox*>(sender())->itemData(i).toInt();
	emit expressionCalculationUpdated(0);
}
void PreferencesDialog::complexFormChanged(int i) {
	settings->evalops.complex_number_form = (ComplexNumberForm) qobject_cast<QComboBox*>(sender())->itemData(i).toInt();
	settings->complex_angle_form = (settings->evalops.complex_number_form == COMPLEX_NUMBER_FORM_CIS);
	emit expressionCalculationUpdated(0);
}
void PreferencesDialog::roundEvenToggled(bool b) {
	settings->printops.round_halfway_to_even = b;
	emit resultFormatUpdated();
}
void PreferencesDialog::mixedUnitsToggled(bool b) {
	if(b) settings->evalops.mixed_units_conversion = MIXED_UNITS_CONVERSION_DEFAULT;
	else settings->evalops.mixed_units_conversion = MIXED_UNITS_CONVERSION_NONE;
	emit expressionCalculationUpdated(0);
}
void PreferencesDialog::conversionChanged(int i) {
	settings->evalops.auto_post_conversion = (AutoPostConversion) qobject_cast<QComboBox*>(sender())->itemData(i).toInt();
	emit expressionCalculationUpdated(0);
}
void PreferencesDialog::prefixesChanged(int index) {
	int i = qobject_cast<QComboBox*>(sender())->itemData(index).toInt();
	settings->prefixes_default = (i == 0);
	settings->printops.use_unit_prefixes = (i == 0 && settings->printops.negative_exponents) || i >= 2;
	settings->printops.use_prefixes_for_all_units = (i == 4);
	settings->printops.use_prefixes_for_currencies = (i >= 3);
	emit resultFormatUpdated();
}
void PreferencesDialog::allPrefixesToggled(bool b) {
	settings->printops.use_all_prefixes = b;
	emit resultFormatUpdated();
}
void PreferencesDialog::denominatorPrefixToggled(bool b) {
	settings->printops.use_denominator_prefix = b;
	emit resultFormatUpdated();
}
void PreferencesDialog::variableUnitsToggled(bool b) {
	CALCULATOR->setVariableUnitsEnabled(b);
	emit expressionCalculationUpdated(0);
}
void PreferencesDialog::groupingChanged(int i) {
	settings->printops.digit_grouping = (DigitGrouping) qobject_cast<QComboBox*>(sender())->itemData(i).toInt();
	emit resultFormatUpdated();
}
void PreferencesDialog::intervalDisplayChanged(int i) {
	if(i == 0) {
		settings->printops.interval_display = INTERVAL_DISPLAY_SIGNIFICANT_DIGITS;
		settings->adaptive_interval_display = true;
	} else {
		settings->printops.interval_display = (IntervalDisplay) qobject_cast<QComboBox*>(sender())->itemData(i).toInt();
		settings->adaptive_interval_display = false;
	}
	emit resultFormatUpdated();
}
void PreferencesDialog::limitImplicitToggled(bool b) {
	settings->evalops.parse_options.limit_implicit_multiplication = b;
	settings->printops.limit_implicit_multiplication = b;
	emit expressionFormatUpdated(true);
}
void PreferencesDialog::titleChanged(int i) {
	settings->title_type = qobject_cast<QComboBox*>(sender())->itemData(i).toInt();
	emit titleTypeChanged();
}
void PreferencesDialog::closeEvent(QCloseEvent *e) {
	QDialog::closeEvent(e);
	emit dialogClosed();
}

