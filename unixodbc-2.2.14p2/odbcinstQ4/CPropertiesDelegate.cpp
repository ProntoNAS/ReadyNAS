/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#include <QtGui>

#include "CPropertiesDelegate.h"
#include "CPropertiesModel.h"
#include "CFileSelector.h"

CPropertiesDelegate::CPropertiesDelegate( QObject *pobjectParent )
    : QItemDelegate( pobjectParent )
{
}

/* we could just load the value now and save all of the subsequent logic to do so - but we follow the Qt encouraged flow of things and defer to setEditorData */
QWidget *CPropertiesDelegate::createEditor( QWidget *pwidgetParent, const QStyleOptionViewItem &styleoptionviewitem, const QModelIndex &modelindex ) const
{
    if ( modelindex.column() != 1 )
        return 0;

    HODBCINSTPROPERTY pProperty = modelindex.model()->data( modelindex, Qt::EditRole ).value<HODBCINSTPROPERTY>();

    switch ( pProperty->nPromptType )
    {
        case ODBCINST_PROMPTTYPE_LABEL:
            {
                QLabel *pLabel = new QLabel( pwidgetParent );
                return pLabel;
            }
            break;

        case ODBCINST_PROMPTTYPE_LISTBOX:
            {
                QComboBox *pComboBox = new QComboBox( pwidgetParent );
                for ( int n = 0; (pProperty->aPromptData)[n]; n++ )
                {
                    pComboBox->insertItem( 0, (pProperty->aPromptData)[n] );
                }
                return pComboBox;
            }
            break;

        case ODBCINST_PROMPTTYPE_COMBOBOX:
            {
                QComboBox *pComboBox = new QComboBox( pwidgetParent );
                for ( int n = 0; (pProperty->aPromptData)[n]; n++ )
                {
                    pComboBox->insertItem( 0, (pProperty->aPromptData)[n] );
                }
                pComboBox->setEditable( true );
                return pComboBox;
            }
            break;

        case ODBCINST_PROMPTTYPE_FILENAME:
            {
                CFileSelector *pFileSelector = new CFileSelector( CFileSelector::Driver, QString::null, true, true, pwidgetParent );
                return pFileSelector;
            }
            break;

        case ODBCINST_PROMPTTYPE_HIDDEN:
            break;

        default: // PROMPTTYPE_TEXTEDIT and PROMPTTYPE_TEXTEDIT_PASSWORD
            {
                QLineEdit *pLineEdit = new QLineEdit( pwidgetParent );
                if ( pProperty->nPromptType == ODBCINST_PROMPTTYPE_TEXTEDIT_PASSWORD )
                    pLineEdit->setEchoMode( QLineEdit::Password ) ;
                return pLineEdit;
            }
            break;
    }

    return 0;
}

void CPropertiesDelegate::setEditorData( QWidget *pwidgetEditor, const QModelIndex &modelindex ) const
{
    HODBCINSTPROPERTY pProperty = modelindex.model()->data( modelindex, Qt::EditRole ).value<HODBCINSTPROPERTY>();

    switch ( pProperty->nPromptType )
    {
        case ODBCINST_PROMPTTYPE_LABEL:
            break;

        case ODBCINST_PROMPTTYPE_LISTBOX:
            {
                QComboBox *pComboBox = static_cast<QComboBox*>( pwidgetEditor );
                pComboBox->setCurrentIndex( pComboBox->findText( pProperty->szValue, Qt::MatchExactly ) );
            }
            break;

        case ODBCINST_PROMPTTYPE_COMBOBOX:
            {
                QComboBox *pComboBox = static_cast<QComboBox*>( pwidgetEditor );
                pComboBox->lineEdit()->setText( pProperty->szValue );
            }
            break;

        case ODBCINST_PROMPTTYPE_FILENAME:
            {
                CFileSelector *pFileSelector = static_cast<CFileSelector*>( pwidgetEditor );
                pFileSelector->setText( pProperty->szValue );
            }
            break;

        case ODBCINST_PROMPTTYPE_HIDDEN:
            break;

        default: // PROMPTTYPE_TEXTEDIT and PROMPTTYPE_TEXTEDIT_PASSWORD
            {
                QLineEdit *pLineEdit = static_cast<QLineEdit*>( pwidgetEditor );
                pLineEdit->setText( pProperty->szValue );
            }
            break;
    }
}

/* we could just update the HODBCINSTPROPERTY value here but we go with the Qt encouraged flow of things and defer to the views setData */
void CPropertiesDelegate::setModelData( QWidget *pwidgetEditor, QAbstractItemModel *abstractitemmodel, const QModelIndex &modelindex) const
{
    HODBCINSTPROPERTY pProperty = abstractitemmodel->data( modelindex, Qt::EditRole ).value<HODBCINSTPROPERTY>();

    switch ( pProperty->nPromptType )
    {
        case ODBCINST_PROMPTTYPE_LABEL:
            break;

        case ODBCINST_PROMPTTYPE_LISTBOX:
            {
                QComboBox *pComboBox = static_cast<QComboBox*>( pwidgetEditor );
                abstractitemmodel->setData( modelindex, pComboBox->currentText(), Qt::EditRole );
            }
            break;

        case ODBCINST_PROMPTTYPE_COMBOBOX:
            {
                QComboBox *pComboBox = static_cast<QComboBox*>( pwidgetEditor );
                abstractitemmodel->setData( modelindex, pComboBox->currentText(), Qt::EditRole );
            }
            break;

        case ODBCINST_PROMPTTYPE_FILENAME:
            {
                CFileSelector *pFileSelector = static_cast<CFileSelector*>( pwidgetEditor );
                abstractitemmodel->setData( modelindex, pFileSelector->getText(), Qt::EditRole );
            }
            break;

        case ODBCINST_PROMPTTYPE_HIDDEN:
            break;

        default: // PROMPTTYPE_TEXTEDIT and PROMPTTYPE_TEXTEDIT_PASSWORD
            {
                QLineEdit *pLineEdit = static_cast<QLineEdit*>( pwidgetEditor );
                abstractitemmodel->setData( modelindex, pLineEdit->text(), Qt::EditRole );
            }
            break;
    }
}

void CPropertiesDelegate::updateEditorGeometry( QWidget *pwidgetEditor, const QStyleOptionViewItem &styleoptionviewitem, const QModelIndex &modelindex ) const
{
    pwidgetEditor->setGeometry( styleoptionviewitem.rect );
}

