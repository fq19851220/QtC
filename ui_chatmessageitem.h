/********************************************************************************
** Form generated from reading UI file '聊天消息_uiTqBjrl.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef _804A__5929__6D88__606F__UITQBJRL_H
#define _804A__5929__6D88__606F__UITQBJRL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatMessageItem
{
public:
    QHBoxLayout *rootHorizontalLayout;
    QLabel *leftAvatarLabel;
    QSpacerItem *leftAvatarBubbleSpacer;
    QLabel *leftArrowLabel;
    QFrame *bubbleFrame;
    QHBoxLayout *bubbleLayout;
    QLabel *messageLabel;
    QLabel *rightArrowLabel;
    QSpacerItem *rightBubbleAvatarSpacer;
    QLabel *rightAvatarLabel;

    void setupUi(QWidget *ChatMessageItem)
    {
        if (ChatMessageItem->objectName().isEmpty())
            ChatMessageItem->setObjectName("ChatMessageItem");
        ChatMessageItem->resize(640, 72);
        ChatMessageItem->setMinimumSize(QSize(0, 56));
        ChatMessageItem->setStyleSheet(QString::fromUtf8("QWidget#ChatMessageItem {\n"
"    background: transparent;\n"
"}\n"
"\n"
"QLabel#leftAvatarLabel,\n"
"QLabel#rightAvatarLabel {\n"
"    border-radius: 4px;\n"
"    background: #D8D8D8;\n"
"}\n"
"\n"
"QFrame#bubbleFrame {\n"
"    background: #FFFFFF;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"QLabel#messageLabel {\n"
"    color: #111111;\n"
"    font-size: 16px;\n"
"    background: transparent;\n"
"}\n"
"\n"
"QLabel#leftArrowLabel,\n"
"QLabel#rightArrowLabel {\n"
"    color: #FFFFFF;\n"
"    background: transparent;\n"
"    font-size: 18px;\n"
"}"));
        rootHorizontalLayout = new QHBoxLayout(ChatMessageItem);
        rootHorizontalLayout->setSpacing(0);
        rootHorizontalLayout->setObjectName("rootHorizontalLayout");
        rootHorizontalLayout->setContentsMargins(12, 6, 12, 6);
        leftAvatarLabel = new QLabel(ChatMessageItem);
        leftAvatarLabel->setObjectName("leftAvatarLabel");
        leftAvatarLabel->setMinimumSize(QSize(42, 42));
        leftAvatarLabel->setMaximumSize(QSize(42, 42));
        leftAvatarLabel->setScaledContents(true);
        leftAvatarLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        rootHorizontalLayout->addWidget(leftAvatarLabel);

        leftAvatarBubbleSpacer = new QSpacerItem(8, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        rootHorizontalLayout->addItem(leftAvatarBubbleSpacer);

        leftArrowLabel = new QLabel(ChatMessageItem);
        leftArrowLabel->setObjectName("leftArrowLabel");
        leftArrowLabel->setMinimumSize(QSize(8, 42));
        leftArrowLabel->setMaximumSize(QSize(8, 42));
        leftArrowLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTop|Qt::AlignmentFlag::AlignTrailing);
        leftArrowLabel->setMargin(8);

        rootHorizontalLayout->addWidget(leftArrowLabel, 0, Qt::AlignmentFlag::AlignTop);

        bubbleFrame = new QFrame(ChatMessageItem);
        bubbleFrame->setObjectName("bubbleFrame");
        bubbleFrame->setMinimumSize(QSize(32, 42));
        bubbleFrame->setMaximumSize(QSize(520, 16777215));
        bubbleFrame->setFrameShape(QFrame::Shape::NoFrame);
        bubbleFrame->setFrameShadow(QFrame::Shadow::Plain);
        bubbleLayout = new QHBoxLayout(bubbleFrame);
        bubbleLayout->setObjectName("bubbleLayout");
        bubbleLayout->setContentsMargins(14, 9, 14, 9);
        messageLabel = new QLabel(bubbleFrame);
        messageLabel->setObjectName("messageLabel");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(messageLabel->sizePolicy().hasHeightForWidth());
        messageLabel->setSizePolicy(sizePolicy);
        messageLabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        messageLabel->setWordWrap(true);
        messageLabel->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse);

        bubbleLayout->addWidget(messageLabel);


        rootHorizontalLayout->addWidget(bubbleFrame);

        rightArrowLabel = new QLabel(ChatMessageItem);
        rightArrowLabel->setObjectName("rightArrowLabel");
        rightArrowLabel->setMinimumSize(QSize(8, 42));
        rightArrowLabel->setMaximumSize(QSize(8, 42));
        rightArrowLabel->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignTop);
        rightArrowLabel->setMargin(8);

        rootHorizontalLayout->addWidget(rightArrowLabel, 0, Qt::AlignmentFlag::AlignTop);

        rightBubbleAvatarSpacer = new QSpacerItem(8, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        rootHorizontalLayout->addItem(rightBubbleAvatarSpacer);

        rightAvatarLabel = new QLabel(ChatMessageItem);
        rightAvatarLabel->setObjectName("rightAvatarLabel");
        rightAvatarLabel->setMinimumSize(QSize(42, 42));
        rightAvatarLabel->setMaximumSize(QSize(42, 42));
        rightAvatarLabel->setScaledContents(true);
        rightAvatarLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        rootHorizontalLayout->addWidget(rightAvatarLabel);


        retranslateUi(ChatMessageItem);

        QMetaObject::connectSlotsByName(ChatMessageItem);
    } // setupUi

    void retranslateUi(QWidget *ChatMessageItem)
    {
        ChatMessageItem->setWindowTitle(QCoreApplication::translate("ChatMessageItem", "ChatMessageItem", nullptr));
        leftAvatarLabel->setText(QString());
        leftArrowLabel->setText(QCoreApplication::translate("ChatMessageItem", "\342\227\200", nullptr));
        messageLabel->setText(QCoreApplication::translate("ChatMessageItem", "\350\277\231\346\230\257\344\270\200\346\235\241\350\201\212\345\244\251\346\266\210\346\201\257", nullptr));
        rightArrowLabel->setText(QCoreApplication::translate("ChatMessageItem", "\342\226\266", nullptr));
        rightAvatarLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ChatMessageItem: public Ui_ChatMessageItem {};
} // namespace Ui

QT_END_NAMESPACE

#endif // _804A__5929__6D88__606F__UITQBJRL_H
