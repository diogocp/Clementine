/* This file is part of Clementine.
   Copyright 2018, Diogo Pereira <sir.suriv@gmail.com>

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LISTENBRAINZCLIENT_H
#define LISTENBRAINZCLIENT_H

#include <QJsonDocument>
#include <QObject>
#include <QtNetwork>

#include "core/song.h"
#include "core/network.h"


class ListenBrainzClient : public QObject {
Q_OBJECT
    // Gets metadata for a particular MBID.
    // An MBID is created from a fingerprint using MusicDnsClient.
    // You can create one MusicBrainzClient and make multiple requests using it.
    // IDs are provided by the caller when a request is started and included in
    // the Finished signal - they have no meaning to MusicBrainzClient.

public:
    // The second argument allows for specifying a custom network access
    // manager. It is used in tests. The ownership of network
    // is not transferred.
    ListenBrainzClient(QObject *parent = nullptr, QNetworkAccessManager *network = nullptr);

    void getListens(QString username);
    void submitListen(const Song& song);
    void playingNow(const Song& song);

private slots:

    void requestFinished();

private:
    const QString apiRootUrl = "http://localhost/1";//"https://api.listenbrainz.org/1";

    QJsonObject trackMetadata(const Song& song);

    QNetworkAccessManager *network_;
    NetworkTimeouts *timeouts_;
};


#endif // LISTENBRAINZCLIENT_H
