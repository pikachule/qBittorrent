/*
 * Bittorrent Client using Qt4 and libtorrent.
 * Copyright (C) 2006  Christophe Dumez
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * Contact : chris@qbittorrent.org
 */
#ifndef __BITTORRENT_H__
#define __BITTORRENT_H__

#include <QHash>
#include <QString>
#include <QStringList>
#include <QTimer>

#include <libtorrent/entry.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/fingerprint.hpp>
#include <libtorrent/session_settings.hpp>
#include <libtorrent/identify_client.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/extensions/metadata_transfer.hpp>
#include <libtorrent/extensions/ut_pex.hpp>
#include <libtorrent/ip_filter.hpp>

#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/exception.hpp>

#include "deleteThread.h"

using namespace libtorrent;
namespace fs = boost::filesystem;

class downloadThread;
#ifndef NO_UPNP
  class CUPnPControlPoint;
  class CUPnPPortMapping;
#endif

class bittorrent : public QObject{
  Q_OBJECT

  private:
    session *s;
    bool DHTEnabled;
#ifndef NO_UPNP
    bool UPnPEnabled;
#endif
    QString scan_dir;
    QTimer *timerScan;
    QTimer *timerAlerts;
    downloadThread *downloader;
    QStringList supported_preview_extensions;
    QString defaultSavePath;
#ifndef NO_UPNP
    CUPnPControlPoint*  m_upnp;
    std::vector<CUPnPPortMapping> m_upnpMappings;
#endif

  protected:
    QString getSavePath(const QString& hash);

  public:
    // Constructor / Destructor
    bittorrent();
    ~bittorrent();
    torrent_handle getTorrentHandle(const QString& hash) const;
    std::vector<torrent_handle> getTorrentHandles() const;
    bool isPaused(const QString& hash) const;
    bool hasFilteredFiles(const QString& fileHash) const;
    bool isFilePreviewPossible(const QString& fileHash) const;
    bool isDHTEnabled() const;
    float getPayloadDownloadRate() const;
    float getPayloadUploadRate() const;
    QList<torrent_handle> getFinishedTorrentHandles() const;
    session_status getSessionStatus() const;
    int getListenPort() const;

  public slots:
    void addTorrent(const QString& path, bool fromScanDir = false, const QString& from_url = QString());
    void downloadFromUrl(const QString& url);
    void downloadFromURLList(const QStringList& url_list);
    void deleteTorrent(const QString& hash, bool permanent = false);
    void pauseTorrent(const QString& hash);
    void pauseAllTorrents();
    void resumeAllTorrents();
    void resumeTorrent(const QString& hash);
    void enableDHT();
    void disableDHT();
    void saveDHTEntry();
    void saveFastResumeData();
    void enableDirectoryScanning(const QString& scan_dir);
    void disableDirectoryScanning();
    void enablePeerExchange();
    void enableIPFilter(ip_filter filter);
    void disableIPFilter();
    void reloadTorrent(const torrent_handle &h, bool compact_mode = true);
    void resumeUnfinishedTorrents();
    // Session configuration - Setters
    void setListeningPortsRange(std::pair<unsigned short, unsigned short> ports);
    void setMaxConnections(int maxConnec);
    void setDownloadRateLimit(int rate);
    void setUploadRateLimit(int rate);
    void setGlobalRatio(float ratio);
    void setDHTPort(int dht_port);
    void setSessionSettings(session_settings sessionSettings);
    void setDefaultSavePath(const QString& savepath);
#ifndef NO_UPNP
    void enableUPnP();
    void disableUPnP();
#endif

  protected slots:
    void cleanDeleter(deleteThread* deleter);
    void loadFilteredFiles(torrent_handle& h);
    void scanDirectory();
    void readAlerts();
    void processDownloadedFile(const QString&, const QString&, int, const QString&);
    void resumeUnfinished();
    void loadTrackerFile(const QString& hash);
    void saveTrackerFile(const QString& hash);

  signals:
    void invalidTorrent(const QString& path);
    void duplicateTorrent(const QString& path);
    void addedTorrent(const QString& path, torrent_handle& h, bool fastResume);
    void finishedTorrent(torrent_handle& h);
    void fullDiskError(torrent_handle& h);
    void trackerError(const QString& hash, const QString& time, const QString& msg);
    void portListeningFailure();
    void trackerAuthenticationRequired(torrent_handle& h);
    void downloadFromUrlFailure(const QString& url, const QString& error);
    void scanDirFoundTorrents(const QStringList& pathList);
    void newDownloadedTorrent(const QString& path, const QString& url);
    void aboutToDownloadFromUrl(const QString& url);
};

#endif
