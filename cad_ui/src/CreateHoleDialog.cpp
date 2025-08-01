#include "cad_ui/CreateHoleDialog.h"
#include <QMessageBox>
#include <QFormLayout>
#pragma execution_character_set("utf-8")

namespace cad_ui {

CreateHoleDialog::CreateHoleDialog(QWidget* parent)
    : QDialog(parent), m_isSelectingFace(false) {
    setupUI();
    setModal(false); 
    setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    resize(380, 500);
}

void CreateHoleDialog::setupUI() {
    setWindowTitle("�ڿײ���");
    // ʹ����ȷ�������滻 auto*
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

	// face selection group
    m_selectionGroup = new QGroupBox("ѡ��", this);
    QGridLayout* selectionLayout = new QGridLayout(m_selectionGroup);

    QLabel* faceLabel = new QLabel("��λ��:", this);
    m_faceStatusLabel = new QLabel("δѡ��", this);
    m_faceStatusLabel->setStyleSheet("color: #888;");
    m_selectFaceButton = new QPushButton("ѡ��", this);

    selectionLayout->addWidget(faceLabel, 0, 0);
    selectionLayout->addWidget(m_faceStatusLabel, 0, 1);
    selectionLayout->addWidget(m_selectFaceButton, 0, 2);

	// parameters group
    m_parametersGroup = new QGroupBox("����", this);
    QFormLayout* parametersLayout = new QFormLayout(m_parametersGroup);

    m_diameterSpinBox = new QDoubleSpinBox(this);
    m_diameterSpinBox->setRange(0.1, 1000.0);
    m_diameterSpinBox->setValue(10.0);
    m_diameterSpinBox->setSuffix(" mm");
    parametersLayout->addRow("ֱ��:", m_diameterSpinBox);

    m_depthSpinBox = new QDoubleSpinBox(this);
    m_depthSpinBox->setRange(0.1, 1000.0);
    m_depthSpinBox->setValue(20.0);
    m_depthSpinBox->setSuffix(" mm");
    parametersLayout->addRow("���:", m_depthSpinBox);

	// botton layout
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_okButton = new QPushButton("ȷ��", this);
    m_cancelButton = new QPushButton("ȡ��", this);
    m_okButton->setEnabled(false); // Ĭ�ϲ����ã�ֱ��ѡ������

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_okButton);

    mainLayout->addWidget(m_selectionGroup);
    mainLayout->addWidget(m_parametersGroup);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

	// connect signals and slots
    connect(m_selectFaceButton, &QPushButton::clicked, this, &CreateHoleDialog::onSelectFaceClicked);
    connect(m_okButton, &QPushButton::clicked, this, &CreateHoleDialog::onAccept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void CreateHoleDialog::onSelectFaceClicked() {
    m_isSelectingFace = true;
    m_selectFaceButton->setText("���");

    emit selectionModeChanged(true, "��ѡ��һ���������ڿ�");

	// switch to selection mode
    disconnect(m_selectFaceButton, &QPushButton::clicked, this, &CreateHoleDialog::onSelectFaceClicked);
    connect(m_selectFaceButton, &QPushButton::clicked, this, &CreateHoleDialog::onSelectionFinished);
}


void CreateHoleDialog::onObjectSelected(const cad_core::ShapePtr& shape) {
    if (m_isSelectingFace) {
        m_targetShape = shape; 
    }
}


void CreateHoleDialog::onFaceSelected(const TopoDS_Face& face) {
    if (m_isSelectingFace) {
        m_selectedFace = face;
        updateSelectionDisplay();
        checkCanAccept();
    }
}

void CreateHoleDialog::onSelectionFinished() {
    m_isSelectingFace = false;
    m_selectFaceButton->setText("ѡ��");

    emit selectionModeChanged(false, "");

	// reset target shape and selected face
    disconnect(m_selectFaceButton, &QPushButton::clicked, this, &CreateHoleDialog::onSelectionFinished);
    connect(m_selectFaceButton, &QPushButton::clicked, this, &CreateHoleDialog::onSelectFaceClicked);
}

void CreateHoleDialog::updateSelectionDisplay() {
    if (!m_selectedFace.IsNull()) {
        m_faceStatusLabel->setText("��ѡ�� 1 ����");
        m_faceStatusLabel->setStyleSheet("color: green;");
    }
    else {
        m_faceStatusLabel->setText("δѡ��");
        m_faceStatusLabel->setStyleSheet("color: #888;");
    }
}

void CreateHoleDialog::checkCanAccept() {
    bool canAccept = m_targetShape && !m_selectedFace.IsNull();
    m_okButton->setEnabled(canAccept);
}

void CreateHoleDialog::onAccept() {
    if (!m_targetShape || m_selectedFace.IsNull()) {
        QMessageBox::warning(this, "����", "����ѡ��һ����Ч���档");
        return;
    }
    double diameter = m_diameterSpinBox->value();
    double depth = m_depthSpinBox->value();
    emit operationRequested(m_targetShape, m_selectedFace, diameter, depth);
    accept();
}

} // namespace cad_ui