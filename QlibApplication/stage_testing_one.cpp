#include "stage_testing_one.h"

stage_testing_one::stage_testing_one(QWidget *parent)
    : QWidget(parent)
{

    label -> setAlignment(Qt::AlignCenter);
    central_layout -> addWidget(label);
    setLayout(central_layout);

}

stage_testing_one::~stage_testing_one()
{
}

