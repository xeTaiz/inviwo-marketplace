/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2020 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#pragma once

#include <modules/qtwidgets/qtwidgetsmoduledefine.h>
#include <inviwo/core/common/inviwo.h>

#include <modules/qtwidgets/ordinalbasewidget.h>
#include <modules/qtwidgets/properties/valuedragspinbox.h>
#include <modules/qtwidgets/properties/doublevaluedragspinbox.h>

#include <modules/qtwidgets/qstringhelper.h>
#include <modules/qtwidgets/inviwoqtutils.h>

#include <warn/push>
#include <warn/ignore/all>
#include <QWidget>
#include <QLocale>
#include <warn/pop>

namespace inviwo {

class IVW_MODULE_QTWIDGETS_API BaseOrdinalSpinBoxWidget : public QWidget {
#include <warn/push>
#include <warn/ignore/all>
    Q_OBJECT
#include <warn/pop>
public:
    BaseOrdinalSpinBoxWidget();
    virtual ~BaseOrdinalSpinBoxWidget();

    void setWrapping(bool wrap);
    bool wrapping() const;

protected:
    virtual double transformValueToEditor() = 0;
    virtual void newEditorValue(double) = 0;
    virtual double minimumValue() = 0;
    virtual double maximumValue() = 0;
    virtual double increment() = 0;
    virtual int spinnerDecimals() = 0;

    void applyInit();
    void applyValue();
    void applyRange();
    void applyIncrement();

    DoubleValueDragSpinBox* editor_;
    ConstraintBehaviour minCB_;
    ConstraintBehaviour maxCB_;

signals:
    void valueChanged();

private:
    void updateFromEditor();
    void updateEditor();
};

template <typename T>
class OrdinalSpinBoxWidget : public BaseOrdinalSpinBoxWidget, public OrdinalBaseWidget<T> {
public:
    OrdinalSpinBoxWidget()
        : BaseOrdinalSpinBoxWidget(), value_(0), minValue_(0), maxValue_(0), increment_(0) {}
    virtual ~OrdinalSpinBoxWidget() = default;

    // Implements OrdinalBaseWidget
    virtual T getValue() const override;
    virtual void setValue(T value) override;
    virtual void initValue(T value) override;
    virtual void setMinValue(T minValue, ConstraintBehaviour cb) override;
    virtual void setMaxValue(T maxValue, ConstraintBehaviour cb) override;
    virtual void setIncrement(T increment) override;

protected:
    // Define the transforms
    T editorToRepr(double val) {
        if constexpr (std::is_floating_point_v<T>) {
            return static_cast<T>(val);
        } else {
            return static_cast<int>(val + 0.5);
        }
    }
    double reprToEditor(T val) {
        if constexpr (std::is_floating_point_v<T>) {
            return static_cast<double>(val);
        } else {
            return static_cast<double>(val);
        }
    }

    // Has implementations using above transformations.
    virtual double transformValueToEditor() override;
    virtual void newEditorValue(double) override;
    virtual double minimumValue() override;
    virtual double maximumValue() override;
    virtual double increment() override;
    virtual int spinnerDecimals() override;

    T value_;
    T minValue_;
    T maxValue_;
    T increment_;
};

template <typename T>
double OrdinalSpinBoxWidget<T>::transformValueToEditor() {
    return reprToEditor(value_);
}
template <typename T>
void OrdinalSpinBoxWidget<T>::newEditorValue(double val) {
    value_ = editorToRepr(val);
}
template <typename T>
double OrdinalSpinBoxWidget<T>::minimumValue() {
    if (minCB_ == ConstraintBehaviour::Ignore) {
        return std::numeric_limits<double>::lowest();
    } else {
        return static_cast<double>(minValue_);
    }
}
template <typename T>
double OrdinalSpinBoxWidget<T>::maximumValue() {
    if (maxCB_ == ConstraintBehaviour::Ignore) {
        return std::numeric_limits<double>::max();
    } else {
        return static_cast<double>(maxValue_);
    }
}
template <typename T>
double OrdinalSpinBoxWidget<T>::increment() {
    return static_cast<double>(increment_);
}
template <typename T>
int OrdinalSpinBoxWidget<T>::spinnerDecimals() {
    return utilqt::decimals<T>(increment());
}
template <typename T>
T OrdinalSpinBoxWidget<T>::getValue() const {
    return value_;
}
template <typename T>
void OrdinalSpinBoxWidget<T>::setValue(T value) {
    if (value != value_) {
        value_ = value;
        applyValue();
    }
}
template <typename T>
void OrdinalSpinBoxWidget<T>::initValue(T value) {
    value_ = value;
    applyInit();
}
template <typename T>
void OrdinalSpinBoxWidget<T>::setMinValue(T minValue, ConstraintBehaviour cb) {
    if (minValue_ != minValue || minCB_ != cb) {
        minValue_ = minValue;
        minCB_ = cb;
        applyRange();
    }
}
template <typename T>
void OrdinalSpinBoxWidget<T>::setMaxValue(T maxValue, ConstraintBehaviour cb) {
    if (maxValue_ != maxValue || maxCB_ != cb) {
        maxValue_ = maxValue;
        maxCB_ = cb;
        applyRange();
    }
}
template <typename T>
void OrdinalSpinBoxWidget<T>::setIncrement(T increment) {
    if (increment_ != increment) {
        increment_ = increment;
        applyIncrement();
    }
}

}  // namespace inviwo
