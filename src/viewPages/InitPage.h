/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef STARTPAGE_H
#define STARTPAGE_H

#include "Page.h"

class QVBoxLayout;
class Error;

namespace Ui
{
class InitPage;
}

// this is the definition of the start page which contains logos, a login component and a start button
// the page as a stackwidget which will switch between login widget and logged widget
// as every page it implements the moveToNextPage and moveToPreviousPage
// the methods onEnter and onExit are called dynamically from the page manager.

class InitPage : public Page
{

    Q_OBJECT

public:

    explicit InitPage(QWidget *parent = 0);
    virtual ~InitPage();

public slots:

    void onInit();
    void onEnter();
    void onExit();

protected slots:

    void slotAuthorizationError(Error*);
    void slotNetworkError(Error*);
    void slotLogOutButton();
    void slotAuthorized();
    void slotLoadData();
    void slotUserLoaded();
    void slotDataLoaded();

protected:

    void setWaiting(bool waiting = true);

private:

    Ui::InitPage *ui;
};

#endif  // STARTPAGE_H //