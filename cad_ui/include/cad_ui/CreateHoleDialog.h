#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include "cad_core/Shape.h"
#include <TopoDS_Face.hxx>

// ǰ������
namespace cad_ui {
    class QtOccView;
}

namespace cad_ui {

class CreateHoleDialog : public QDialog {
    Q_OBJECT

public:
    explicit CreateHoleDialog(QWidget* parent = nullptr);
    ~CreateHoleDialog() = default;

    void onObjectSelected(const cad_core::ShapePtr& shape);
    void onFaceSelected(const TopoDS_Face& face);

signals:   
    void operationRequested(const cad_core::ShapePtr& targetShape, const TopoDS_Face& selectedFace, double diameter, double depth);
    void selectionModeChanged(bool enabled, const QString& prompt);

private slots:
    void onSelectFaceClicked();
    void onSelectionFinished();
    void onAccept();

private:
    void setupUI();
    void updateSelectionDisplay();
    void checkCanAccept();

    // ״̬����
    cad_core::ShapePtr m_targetShape;   // �洢��ѡ�е�ʵ��
    TopoDS_Face m_selectedFace;         // �洢��ѡ�е���
    bool m_isSelectingFace;             // ����Ƿ������ڡ�ѡ���桱��״̬

    // UI �ؼ�
    QGroupBox* m_selectionGroup;
    QPushButton* m_selectFaceButton;
    QLabel* m_faceStatusLabel;

    QGroupBox* m_parametersGroup;
    QDoubleSpinBox* m_diameterSpinBox;
    QDoubleSpinBox* m_depthSpinBox;

    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
};

} // namespace cad_ui#pragma once
