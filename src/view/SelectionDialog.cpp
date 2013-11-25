/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "SelectionDialog.h"
#include "ui_selectiondialog.h"

SelectionDialog::SelectionDialog(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::SelectionDialog),
    m_includeAmbiguous(false), m_caseSensitive(false), m_regExpValid(false)
{
    ui->setupUi(this);
    // set default state
    slotCaseSensitive(false);
    slotIncludeAmbiguous(false);
}

SelectionDialog::~SelectionDialog()
{
    delete ui;
}

const SelectionDialog::GeneList& SelectionDialog::selectedGenes() const
{
    return m_selectedGeneList;
}

const SelectionDialog::GeneList SelectionDialog::selectGenes(QWidget *parent)
{
    SelectionDialog dialog(parent);
    if (dialog.exec() == QDialog::Accepted) {
        return dialog.selectedGenes();
    }
    return SelectionDialog::GeneList();
}

void SelectionDialog::accept()
{
    // early out, should "never" happen
    if (!m_regExp.isValid()) {
        reject();
        return;
    }
    DataProxy *dataProxy = DataProxy::getInstance();
    DataProxy::GeneListPtr geneList = dataProxy->getGeneList(dataProxy->getSelectedDataset());
    // find all genes that match the regular expression
    //TODO this can be optimized using STL functions or Qt::concurrent algorithms
    m_selectedGeneList.clear();
    foreach(DataProxy::GenePtr gene, (*geneList)) {
        const QString name = gene->name();
        if (!m_includeAmbiguous && gene->isAmbiguous()) {
            continue;
        }
        if (name.contains(m_regExp)) {
            m_selectedGeneList.append(gene);
        }
    }
    // and propagate accept call
    QDialog::accept();
}

void SelectionDialog::slotValidateRegExp(const QString &pattern)
{
    m_regExp.setPattern(pattern);
    const bool regExpValid = m_regExp.isValid();
    if (regExpValid != m_regExpValid) {
        m_regExpValid = regExpValid;
        emit signalValidRegExp(m_regExpValid);
    }
}

void SelectionDialog::slotIncludeAmbiguous(bool includeAmbiguous)
{
    m_includeAmbiguous = includeAmbiguous;
    if (m_includeAmbiguous != ui->checkAmbiguous->isChecked()) {
        ui->checkAmbiguous->setChecked(m_includeAmbiguous);
    }
}

void SelectionDialog::slotCaseSensitive(bool caseSensitive)
{
    // toggle case sensitive
    m_regExp.setCaseSensitivity((caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive));

    m_caseSensitive = caseSensitive;
    if (m_caseSensitive != ui->checkCaseSense->isChecked()) {
        ui->checkCaseSense->setChecked(m_caseSensitive);
    }
}

void SelectionDialog::slotEnableAcceptAction(bool enableAcceptAction)
{
    // disable any "accept/yes/apply" type buttons when regexp is invalid
    foreach(QAbstractButton * button, ui->buttonBox->buttons()) {
        const QDialogButtonBox::ButtonRole role = ui->buttonBox->buttonRole(button);
        if (role == QDialogButtonBox::AcceptRole ||
            role == QDialogButtonBox::YesRole ||
            role == QDialogButtonBox::ApplyRole) {
            button->setEnabled(enableAcceptAction);
        }
    }
}
