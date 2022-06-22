#include "setting_line_one.h"

setting_line_one::setting_line_one(QWidget *parent) : QWidget(parent) {

    label -> setAlignment(Qt::AlignCenter);
    central_layout -> addWidget(label);
    setLayout(central_layout);

}

setting_line_one::~setting_line_one() {}

