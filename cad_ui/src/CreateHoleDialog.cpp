#include "cad_ui/CreateHoleDialog.h"
#include "cad_ui/QtOccView.h"
#include <QMessageBox>
#include <QFormLayout>
#pragma execution_character_set("utf-8")

namespace cad_ui {

    CreateHoleDialog::CreateHoleDialog(QtOccView* viewer, QWidget* parent)
        : QDialog(parent), m_isSelectingFace(false), m_viewer(viewer) {
        setupUI();
        setModal(false);
        setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
        resize(380, 500);
        
    }

    // ʵ������������ȷ���Ի���ر�ʱ�ָ�ʵ�����
    CreateHoleDialog::~CreateHoleDialog() {
        if (m_viewer && m_transparentShape) {
            m_viewer->ResetShapeDisplay(m_transparentShape);
        }
    }

void CreateHoleDialog::setupUI() {
    setWindowTitle("�ڿײ���");
    // ʹ����ȷ�������滻 auto*
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

	// face selection group
    m_selectionGroup = new QGroupBox("ѡ��", this);
    auto* selectionLayout = new QVBoxLayout(m_selectionGroup); 

    m_selectFaceButton = new QPushButton("ѡ��Ҫ�ڿ׵���", this);
    m_selectionList = new QListWidget(this);
    m_selectionList->setMaximumHeight(60); // �����б�߶�
    m_selectionList->addItem("��δѡ����...");
    m_selectionList->setStyleSheet("QListWidget::item { color: #888; }");

    selectionLayout->addWidget(m_selectFaceButton);
    selectionLayout->addWidget(m_selectionList);

	// parameters group
    m_parametersGroup = new QGroupBox("����", this);
    QFormLayout* parametersLayout = new QFormLayout(m_parametersGroup);

    m_diameterSpinBox = new QDoubleSpinBox(this);
    m_diameterSpinBox->setRange(0.1, 1000.0);
    m_diameterSpinBox->setValue(5.0);
    m_diameterSpinBox->setSuffix(" mm");
    parametersLayout->addRow("ֱ��:", m_diameterSpinBox);

    m_depthSpinBox = new QDoubleSpinBox(this);
    m_depthSpinBox->setRange(0.1, 1000.0);
    m_depthSpinBox->setValue(5.0);
    m_depthSpinBox->setSuffix(" mm");
    parametersLayout->addRow("���:", m_depthSpinBox);

    parametersLayout->addRow(new QLabel("--- ���������� ---")); 
    m_xCoordSpinBox = new QDoubleSpinBox(this);
    m_xCoordSpinBox->setRange(-1000.0, 1000.0);
    m_xCoordSpinBox->setValue(0.0);
    parametersLayout->addRow("���� X:", m_xCoordSpinBox);

    m_yCoordSpinBox = new QDoubleSpinBox(this);
    m_yCoordSpinBox->setRange(-1000.0, 1000.0);
    m_yCoordSpinBox->setValue(0.0);
    parametersLayout->addRow("���� Y:", m_yCoordSpinBox);

    m_zCoordSpinBox = new QDoubleSpinBox(this);
    m_zCoordSpinBox->setRange(-1000.0, 1000.0);
    m_zCoordSpinBox->setValue(0.0);
    parametersLayout->addRow("���� Z:", m_zCoordSpinBox);

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

void CreateHoleDialog::cleanupAndRestoreView() {
    if (m_viewer && m_transparentShape) {
        m_viewer->ResetShapeDisplay(m_transparentShape);
    }
    // �ؼ������ָ�룬�ж����ʵ����κ���ϵ
    m_transparentShape = nullptr;
}

void CreateHoleDialog::onObjectSelected(const cad_core::ShapePtr& shape) {
    if (m_isSelectingFace) {
        m_targetShape = shape; 
    }
}


void CreateHoleDialog::onFaceSelected(const TopoDS_Face& face) {
    if (m_isSelectingFace) {
        m_selectedFace = face;

        // �ؼ��߼���
        // ��ʱ m_targetShape Ӧ���Ѿ��� onObjectSelected ����Ϊ��ǰ��ĸ�ʵ�塣
        if (m_viewer && m_targetShape) {

            // 1. ���֮ǰ�б��ʵ���ǰ�͸���ģ��Ȱ����ָ�ԭ״
            if (m_transparentShape && m_transparentShape != m_targetShape) {
                m_viewer->ResetShapeDisplay(m_transparentShape);
            }

            // 2. ����ǰ��������ʵ������Ϊ��͸��
            m_viewer->SetShapeTransparency(m_targetShape, 0.5); // ����Ϊ50%͸����

            // 3. ��¼���ĸ�ʵ�屻���Ǳ�͸���ˣ��Ա�֮��ָ�
            m_transparentShape = m_targetShape;
        }

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
    m_selectionList->clear();
    if (!m_selectedFace.IsNull()) {
        m_selectionList->addItem("��ѡ�� 1 ����");
        m_selectionList->setStyleSheet("QListWidget::item { color: green; }");
    }
    else {
        m_selectionList->addItem("��δѡ����...");
        m_selectionList->setStyleSheet("QListWidget::item { color: #888; }");
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

    // ��ȡ����ֵ
    double x = m_xCoordSpinBox->value();
    double y = m_yCoordSpinBox->value();
    double z = m_zCoordSpinBox->value();

    // �������������Ϣ�����ź�
    emit operationRequested(m_targetShape, m_selectedFace, diameter, depth, x, y, z);
    accept();
}

void CreateHoleDialog::updateCenterCoords(double x, double y, double z) {
    m_xCoordSpinBox->setValue(x);
    m_yCoordSpinBox->setValue(y);
    m_zCoordSpinBox->setValue(z);
}

} // namespace cad_ui