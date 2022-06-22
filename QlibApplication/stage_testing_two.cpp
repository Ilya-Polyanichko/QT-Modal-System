#include "stage_testing_two.h"

stage_testing_two::stage_testing_two(QWidget *parent)
    : QWidget(parent)
{

    label -> setAlignment(Qt::AlignCenter);
    central_layout -> addWidget(label);
    setLayout(central_layout);

}

stage_testing_two::~stage_testing_two()
{
}

