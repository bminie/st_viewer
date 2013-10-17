/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H

#include <QObject>
#include <QPointer>
#include <QErrorMessage>

#include "utils/Singleton.h"
#include "controller/error/Error.h"

// a singleton based error manager that shows an error popup with the information 
class ErrorManager : public QObject, public Singleton<ErrorManager>
{
    Q_OBJECT

public:
    
    explicit ErrorManager(QObject* parent = 0);
    virtual ~ErrorManager();

    void finalize();
    void init(QWidget* parentContainer = 0);

public slots:
    
    void slotHandleError(Error* error);

private:
    
    Q_DISABLE_COPY(ErrorManager);
    
    QPointer<QErrorMessage> m_errorDialog;
    QPointer<QWidget> m_parentContainer;
};

#endif // ERRORMANAGER_H //
