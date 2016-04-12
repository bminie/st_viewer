#ifndef AUTHORIZATIONMANAGER_H
#define AUTHORIZATIONMANAGER_H

#include <QObject>
#include <QPointer>
#include <QUuid>

#include "auth/TokenStorage.h"

class NetworkManager;
class OAuth2;
class QWidget;
class Error;
class DataProxy;

//  This class gives an interface for OAuth2 authorization
//  trough the objects OAuth2 and TokenStorage.
//  It emits signals for error, authorized and abortion.
//  It allows for local storage of access token.
class AuthorizationManager : public QObject
{
    Q_OBJECT

public:
    AuthorizationManager(QPointer<DataProxy> dataProxy, QObject* parent = 0);
    virtual ~AuthorizationManager();

    // Start the login process
    void startAuthorization();

    // Clean access token
    void cleanAccesToken();

    // True if the user is already authenticated
    bool isAuthenticated() const;

    // User acces token methods to check
    // if the user is already logged in
    bool hasAccessToken() const;
    QUuid getAccessToken() const;

signals:
    // The login is successful
    void signalAuthorize();
    // There was an error trying to log in
    void signalError(QSharedPointer<Error> error);

private slots:
    // Internal slot to process login object from OAuth2
    void slotLoginDone(const QUuid& accessToken, int expiresIn, const QUuid& refreshToken);

private:
    QPointer<OAuth2> m_oAuth2;
    TokenStorage m_tokenStorage;

    // reference to dataProxy (to be passed along to OAuth2)
    QPointer<DataProxy> m_dataProxy;

    Q_DISABLE_COPY(AuthorizationManager)
};

#endif /* AUTHORIZATIONMANAGER_H */
