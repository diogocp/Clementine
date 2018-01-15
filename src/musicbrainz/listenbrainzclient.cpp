#include "core/song.h"
#include "core/network.h"
#include "core/logging.h"
#include "listenbrainzclient.h"

ListenBrainzClient::ListenBrainzClient(QObject *parent, QNetworkAccessManager *network) :
        QObject(parent),
        network_(network ? network : new NetworkAccessManager(this)),
        timeouts_(new NetworkTimeouts(5000, this)) {}

void ListenBrainzClient::getListens(QString username) {
    QString mbid = QString("/user/%1/listens").arg("iliekcomputers");
    QUrl url(apiRootUrl + mbid);

    QNetworkReply *reply = network_->get(QNetworkRequest(url));

    connect(reply, &QNetworkReply::finished, this, &ListenBrainzClient::requestFinished);

    timeouts_->AddReply(reply);
}

void ListenBrainzClient::submitListen(const Song &song) {
    QJsonObject doc, payload;

    doc["listen_type"] = "single";
    payload["listened_at"] = 0;
    payload["track_metadata"] = trackMetadata(song);
    doc["payload"] = QJsonArray() + payload;

    qLog(Debug) << "ListenBrainz listen: " << QJsonDocument(doc).toJson(QJsonDocument::Compact);
}

void ListenBrainzClient::playingNow(const Song &song) {
    QJsonObject doc, payload;

    doc["listen_type"] = "playing_now";
    payload["track_metadata"] = trackMetadata(song);
    doc["payload"] = QJsonArray() + payload;

    qLog(Debug) << "ListenBrainz playing now: " << QJsonDocument(doc).toJson(QJsonDocument::Compact);
}

QJsonObject ListenBrainzClient::trackMetadata(const Song &song) {
    QJsonObject track_metadata, additional_info;

    track_metadata["artist_name"] = song.artist();
    track_metadata["track_name"] = song.title();
    track_metadata["release_name"] = song.album();

    additional_info["tracknumber"] = song.track();
    additional_info["release_artist"] = song.albumartist();

    track_metadata["additional_info"] = additional_info;

    return track_metadata;
}

void ListenBrainzClient::requestFinished() {
    QNetworkReply *reply = dynamic_cast<QNetworkReply *>(sender());

    reply->deleteLater();

    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
        QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
        qLog(Info) << json;
    } else {
        qLog(Error) << "Error:"
                    << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute)
                            .toInt() << "http status code received";
        qLog(Error) << reply->readAll();
    }
}