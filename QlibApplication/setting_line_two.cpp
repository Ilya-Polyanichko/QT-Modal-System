#include "setting_line_two.h"

setting_line_two::setting_line_two(QWidget *parent)
    : QWidget(parent)
{

    label -> setAlignment(Qt::AlignCenter);
    central_layout -> addWidget(label);
    setLayout(central_layout);

}

setting_line_two::~setting_line_two()
{
}

