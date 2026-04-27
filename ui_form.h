/********************************************************************************
** Form generated from reading UI file '最近联系人_uiqjxazV.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef _6700__8FD1__8054__7CFB__4EBA__UIQJXAZV_H
#define _6700__8FD1__8054__7CFB__4EBA__UIQJXAZV_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *label最近联系人头像;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label最近联系人昵称;
    QSpacerItem *horizontalSpacer;
    QLabel *label最近联系人时间;
    QLabel *label最近联系人消息;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName("Form");
        Form->resize(300, 80);
        Form->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        horizontalLayout = new QHBoxLayout(Form);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(12, 0, 12, 0);
        label最近联系人头像 = new QLabel(Form);
        label最近联系人头像->setObjectName("label\346\234\200\350\277\221\350\201\224\347\263\273\344\272\272\345\244\264\345\203\217");
        label最近联系人头像->setMinimumSize(QSize(50, 50));
        label最近联系人头像->setMaximumSize(QSize(50, 50));
        label最近联系人头像->setScaledContents(true);

        horizontalLayout->addWidget(label最近联系人头像, 0, Qt::AlignmentFlag::AlignVCenter);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(12, 12, 0, 12);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label最近联系人昵称 = new QLabel(Form);
        label最近联系人昵称->setObjectName("label\346\234\200\350\277\221\350\201\224\347\263\273\344\272\272\346\230\265\347\247\260");
        QFont font;
        font.setPointSize(10);
        label最近联系人昵称->setFont(font);
        label最近联系人昵称->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_2->addWidget(label最近联系人昵称);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        label最近联系人时间 = new QLabel(Form);
        label最近联系人时间->setObjectName("label\346\234\200\350\277\221\350\201\224\347\263\273\344\272\272\346\227\266\351\227\264");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Microsoft YaHei UI")});
        font1.setPointSize(8);
        label最近联系人时间->setFont(font1);
        label最近联系人时间->setStyleSheet(QString::fromUtf8("color: #b2b2b2;"));
        label最近联系人时间->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_2->addWidget(label最近联系人时间);


        verticalLayout->addLayout(horizontalLayout_2);

        label最近联系人消息 = new QLabel(Form);
        label最近联系人消息->setObjectName("label\346\234\200\350\277\221\350\201\224\347\263\273\344\272\272\346\266\210\346\201\257");
        QFont font2;
        font2.setPointSize(9);
        label最近联系人消息->setFont(font2);
        label最近联系人消息->setStyleSheet(QString::fromUtf8("color: #999999;"));
        label最近联系人消息->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        verticalLayout->addWidget(label最近联系人消息);


        horizontalLayout->addLayout(verticalLayout);


        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QCoreApplication::translate("Form", "Form", nullptr));
        label最近联系人头像->setText(QString());
        label最近联系人昵称->setText(QString());
        label最近联系人时间->setText(QString());
        label最近联系人消息->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // _6700__8FD1__8054__7CFB__4EBA__UIQJXAZV_H
