/*
 * Wazuh SysCollector
 * Copyright (C) 2015-2020, Wazuh Inc.
 * October 7, 2020.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */
#include "syscollector.hpp"
#include "json.hpp"
#include <iostream>
#include "stringHelper.h"
#include "hashHelper.h"

constexpr auto HOTFIXES_SQL_STATEMENT
{
    R"(CREATE TABLE dbsync_hotfixes(
    hotfix TEXT,
    checksum TEXT,
    PRIMARY KEY (hotfix)) WITHOUT ROWID;)"
};

constexpr auto HOTFIXES_START_CONFIG_STATEMENT
{
    R"({"table":"dbsync_hotfixes",
        "first_query":
            {
                "column_list":["hotfix"],
                "row_filter":" ",
                "distinct_opt":false,
                "order_by_opt":"hotfix ASC",
                "count_opt":1
            },
        "last_query":
            {
                "column_list":["hotfix"],
                "row_filter":" ",
                "distinct_opt":false,
                "order_by_opt":"hotfix DESC",
                "count_opt":1
            },
        "component":"dbsync_hotfixes",
        "index":"hotfix",
        "last_event":"last_event",
        "checksum_field":"checksum",
        "range_checksum_query_json":
            {
                "row_filter":"WHERE hotfix BETWEEN '?' and '?' ORDER BY hotfix",
                "column_list":["hotfix, checksum"],
                "distinct_opt":false,
                "order_by_opt":"",
                "count_opt":100
            }
        })"
};

constexpr auto PACKAGES_SQL_STATEMENT
{
    R"(CREATE TABLE dbsync_packages(
    name TEXT,
    version TEXT,
    vendor TEXT,
    install_time TEXT,
    location TEXT,
    architecture TEXT,
    groups TEXT,
    description TEXT,
    size TEXT,
    priority TEXT,
    multiarch TEXT,
    source TEXT,
    checksum TEXT,
    PRIMARY KEY (name,version,architecture)) WITHOUT ROWID;)"
};

constexpr auto PACKAGES_START_CONFIG_STATEMENT
{
    R"({"table":"dbsync_packages",
        "first_query":
            {
                "column_list":["name"],
                "row_filter":" ",
                "distinct_opt":false,
                "order_by_opt":"name ASC",
                "count_opt":1
            },
        "last_query":
            {
                "column_list":["name"],
                "row_filter":" ",
                "distinct_opt":false,
                "order_by_opt":"name DESC",
                "count_opt":1
            },
        "component":"dbsync_packages",
        "index":"name",
        "last_event":"last_event",
        "checksum_field":"checksum",
        "range_checksum_query_json":
            {
                "row_filter":"WHERE name BETWEEN '?' and '?' ORDER BY name",
                "column_list":["name, checksum"],
                "distinct_opt":false,
                "order_by_opt":"",
                "count_opt":100
            }
        })"
};

constexpr auto PROCESSES_START_CONFIG_STATEMENT
{
    R"({"table":"dbsync_processes",
        "first_query":
            {
                "column_list":["pid"],
                "row_filter":" ",
                "distinct_opt":false,
                "order_by_opt":"pid DESC",
                "count_opt":1
            },
        "last_query":
            {
                "column_list":["pid"],
                "row_filter":" ",
                "distinct_opt":false,
                "order_by_opt":"pid ASC",
                "count_opt":1
            },
        "component":"dbsync_processes",
        "index":"pid",
        "last_event":"last_event",
        "checksum_field":"checksum",
        "range_checksum_query_json":
            {
                "row_filter":"WHERE pid BETWEEN '?' and '?' ORDER BY pid",
                "column_list":["pid, checksum"],
                "distinct_opt":false,
                "order_by_opt":"",
                "count_opt":1000
            }
        })"
};

constexpr auto PROCESSES_SQL_STATEMENT
{
    R"(CREATE TABLE dbsync_processes (
    pid BIGINT,
    name TEXT,
    state TEXT,
    ppid BIGINT,
    utime BIGINT,
    stime BIGINT,
    cmd TEXT,
    argvs TEXT,
    euser TEXT,
    ruser TEXT,
    suser TEXT,
    egroup TEXT,
    rgroup TEXT,
    sgroup TEXT,
    fgroup TEXT,
    priority BIGINT,
    nice BIGINT,
    size BIGINT,
    vm_size BIGINT,
    resident BIGINT,
    share BIGINT,
    start_time BIGINT,
    pgrp BIGINT,
    session BIGINT,
    nlwp BIGINT,
    tgid BIGINT,
    tty BIGINT,
    processor BIGINT,
    checksum TEXT,
    PRIMARY KEY (pid)) WITHOUT ROWID;)"
};

constexpr auto PORTS_START_CONFIG_STATEMENT
{
    R"({"table":"dbsync_ports",
        "first_query":
            {
                "column_list":["inode"],
                "row_filter":" ",
                "distinct_opt":false,
                "order_by_opt":"inode DESC",
                "count_opt":1
            },
        "last_query":
            {
                "column_list":["inode"],
                "row_filter":" ",
                "distinct_opt":false,
                "order_by_opt":"inode ASC",
                "count_opt":1
            },
        "component":"dbsync_ports",
        "index":"inode",
        "last_event":"last_event",
        "checksum_field":"checksum",
        "range_checksum_query_json":
            {
                "row_filter":"WHERE inode BETWEEN '?' and '?' ORDER BY inode",
                "column_list":["inode, checksum"],
                "distinct_opt":false,
                "order_by_opt":"",
                "count_opt":1000
            }
        })"
};

constexpr auto PORTS_SQL_STATEMENT
{
    R"(CREATE TABLE dbsync_ports (
       protocol TEXT,
       local_ip TEXT,
       local_port BIGINT,
       remote_ip TEXT,
       remote_port BIGINT,
       tx_queue BIGINT,
       rx_queue BIGINT,
       inode BIGINT,
       state TEXT,
       pid BIGINT,
       process_name TEXT,
       checksum TEXT,
       PRIMARY KEY (inode, protocol, local_port)) WITHOUT ROWID;)"
};

constexpr auto NETIFACE_START_CONFIG_STATEMENT
{
    R"({"table":"dbsync_network_iface",
        "first_query":
            {
                "column_list":["name"],
                "row_filter":" ",
                "distinct_opt":false,
                "order_by_opt":"name DESC",
                "count_opt":1
            },
        "last_query":
            {
                "column_list":["name"],
                "row_filter":" ",
                "distinct_opt":false,
                "order_by_opt":"name ASC",
                "count_opt":1
            },
        "component":"dbsync_network_iface",
        "index":"name",
        "last_event":"last_event",
        "checksum_field":"checksum",
        "range_checksum_query_json":
            {
                "row_filter":"WHERE name BETWEEN '?' and '?' ORDER BY name",
                "column_list":["name, checksum"],
                "distinct_opt":false,
                "order_by_opt":"",
                "count_opt":1000
            }
        })"
};

constexpr auto NETIFACE_SQL_STATEMENT
{
    R"(CREATE TABLE dbsync_network_iface (
       name TEXT,
       adapter TEXT,
       type TEXT,
       state TEXT,
       mtu TEXT,
       mac TEXT,
       tx_packets INTEGER,
       rx_packets INTEGER,
       tx_bytes INTEGER,
       rx_bytes INTEGER,
       tx_errors INTEGER,
       rx_errors INTEGER,
       tx_dropped INTEGER,
       rx_dropped INTEGER,
       checksum TEXT,
       PRIMARY KEY (name)) WITHOUT ROWID;)"
};

constexpr auto NETPROTO_START_CONFIG_STATEMENT
{
    R"({"table":"dbsync_network_protocol",
        "first_query":
            {
                "column_list":["iface"],
                "row_filter":" ",
                "distinct_opt":false,
                "order_by_opt":"iface DESC",
                "count_opt":1
            },
        "last_query":
            {
                "column_list":["iface"],
                "row_filter":" ",
                "distinct_opt":false,
                "order_by_opt":"iface ASC",
                "count_opt":1
            },
        "component":"dbsync_network_protocol",
        "index":"iface",
        "last_event":"last_event",
        "checksum_field":"checksum",
        "range_checksum_query_json":
            {
                "row_filter":"WHERE iface BETWEEN '?' and '?' ORDER BY iface",
                "column_list":["iface, checksum"],
                "distinct_opt":false,
                "order_by_opt":"",
                "count_opt":1000
            }
        })"
};

constexpr auto NETPROTO_SQL_STATEMENT
{
    R"(CREATE TABLE dbsync_network_protocol (
       iface TEXT,
       type TEXT,
       gateway TEXT,
       dhcp TEXT NOT NULL CHECK (dhcp IN ('enabled', 'disabled', 'unknown', 'BOOTP')) DEFAULT 'unknown',
       metric TEXT,
       checksum TEXT,
       PRIMARY KEY (iface,type)) WITHOUT ROWID;)"
};

constexpr auto NETADDRESS_START_CONFIG_STATEMENT
{
    R"({"table":"dbsync_network_address",
        "first_query":
            {
                "column_list":["iface"],
                "row_filter":" ",
                "distinct_opt":false,
                "order_by_opt":"iface DESC",
                "count_opt":1
            },
        "last_query":
            {
                "column_list":["iface"],
                "row_filter":" ",
                "distinct_opt":false,
                "order_by_opt":"iface ASC",
                "count_opt":1
            },
        "component":"dbsync_network_address",
        "index":"iface",
        "last_event":"last_event",
        "checksum_field":"checksum",
        "range_checksum_query_json":
            {
                "row_filter":"WHERE iface BETWEEN '?' and '?' ORDER BY iface",
                "column_list":["iface, checksum"],
                "distinct_opt":false,
                "order_by_opt":"",
                "count_opt":1000
            }
        })"
};

constexpr auto NETADDR_SQL_STATEMENT
{
    R"(CREATE TABLE dbsync_network_address (
       iface TEXT,
       proto TEXT,
       address TEXT,
       netmask TEXT,
       broadcast TEXT,
       checksum TEXT,
       PRIMARY KEY (iface,proto,address)) WITHOUT ROWID;)"
};

static void updateAndNotifyChanges(const DBSYNC_HANDLE handle,
                                   const std::string& table,
                                   const nlohmann::json& values,
                                   const std::function<void(const std::string&)> reportFunction)
{
    const std::map<ReturnTypeCallback, std::string> operationsMap
    {
        // LCOV_EXCL_START
        {MODIFIED, "MODIFIED"},
        {DELETED , "DELETED "},
        {INSERTED, "INSERTED"},
        {MAX_ROWS, "MAX_ROWS"},
        {DB_ERROR, "DB_ERROR"},
        {SELECTED, "SELECTED"},
        // LCOV_EXCL_STOP
    };
    constexpr auto queueSize{4096};
    const auto callback
    {
        [&table, &operationsMap, reportFunction](ReturnTypeCallback result, const nlohmann::json& data)
        {
            if (data.is_array())
            {
                for (const auto& item : data)
                {
                    nlohmann::json msg;
                    msg["type"] = table;
                    msg["operation"] = operationsMap.at(result);
                    msg["data"] = item;
                    reportFunction(msg.dump());
                }
            }
            else
            {
                // LCOV_EXCL_START
                nlohmann::json msg;
                msg["type"] = table;
                msg["operation"] = operationsMap.at(result);
                msg["data"] = data;
                reportFunction(msg.dump());
                // LCOV_EXCL_STOP
            }
        }
    };
    DBSyncTxn txn
    {
        handle,
        nlohmann::json{table},
        0,
        queueSize,
        callback
    };
    nlohmann::json jsResult;
    nlohmann::json input;
    input["table"] = table;
    input["data"] = values;
    for (auto& item : input["data"])
    {
        const auto content{item.dump()};
        Utils::HashData hash;
        hash.update(content.c_str(), content.size());
        item["checksum"] = Utils::asciiToHex(hash.hash());
    }
    txn.syncTxnRow(input);
    txn.getDeletedRows(callback);
}

bool Syscollector::sleepFor()
{
    std::unique_lock<std::mutex> lock{m_mutex};
    return !m_cv.wait_for(lock, std::chrono::seconds{m_intervalValue}, [&](){return m_running;});
}

std::string Syscollector::getCreateStatement() const
{
    std::string ret;
    if (m_packages)
    {
        ret += PACKAGES_SQL_STATEMENT;
        if (m_hotfixes)
        {
            ret += HOTFIXES_SQL_STATEMENT;
        }
    }
    if (m_processes)
    {
        ret += PROCESSES_SQL_STATEMENT;
    }
    if (m_ports)
    {
        ret += PORTS_SQL_STATEMENT;
    }
    if (m_network)
    {
        ret += NETIFACE_SQL_STATEMENT;
        ret += NETPROTO_SQL_STATEMENT;
        ret += NETADDR_SQL_STATEMENT;
    }
    return ret;
}

void Syscollector::init(const std::shared_ptr<ISysInfo>& spInfo,
                        const std::function<void(const std::string&)> reportFunction,
                        const unsigned int interval,
                        const bool scanOnStart,
                        const bool hardware,
                        const bool os,
                        const bool network,
                        const bool packages,
                        const bool ports,
                        const bool portsAll,
                        const bool processes,
                        const bool hotfixes)
{
    m_spInfo = spInfo;
    m_reportFunction = reportFunction;
    m_intervalValue = interval;
    m_scanOnStart = scanOnStart;
    m_hardware = hardware;
    m_os = os;
    m_network = network;
    m_packages = packages;
    m_ports = ports;
    m_portsAll = portsAll;
    m_processes = processes;
    m_hotfixes = hotfixes;
    m_running = false;
    m_spDBSync = std::make_unique<DBSync>(HostType::AGENT, DbEngineType::SQLITE3, "syscollector.db", getCreateStatement());
    m_spRsync = std::make_unique<RemoteSync>();
    syncLoop();
}

void Syscollector::destroy()
{
    std::unique_lock<std::mutex> lock{m_mutex};
    m_running = true;
    m_cv.notify_all();
    lock.unlock();
}
void Syscollector::scanHardware()
{
    if (m_hardware)
    {
        nlohmann::json msg;
        msg["type"] = "dbsync_hardware";
        msg["operation"] = "MODIFIED";
        msg["data"] = m_spInfo->hardware();
        m_reportFunction(msg.dump());
    }
}
void Syscollector::scanOs()
{
    if(m_os)
    {
        nlohmann::json msg;
        msg["type"] = "dbsync_os";
        msg["operation"] = "MODIFIED";
        msg["data"] = m_spInfo->os();
        m_reportFunction(msg.dump());
    }
}

void Syscollector::scanNetwork()
{
    if (m_network)
    {
        constexpr auto netIfaceTable    { "dbsync_network_iface"    };
        constexpr auto netProtocolTable { "dbsync_network_protocol" };
        constexpr auto netAddressTable  { "dbsync_network_address"  };
        const auto& networks { m_spInfo->networks() };
        nlohmann::json ifaceTableDataList {};
        nlohmann::json protoTableDataList {};
        nlohmann::json addressTableDataList {};

        if (!networks.is_null())
        {
            const auto& itIface { networks.find("iface") };

            if (networks.end() != itIface)
            {
                for (const auto& item : itIface.value())
                {
                    // Split the resulting networks data into the specific DB tables
                    // "dbsync_network_iface" table data to update and notify
                    nlohmann::json ifaceTableData {};
                    ifaceTableData["name"]       = item.at("name");
                    ifaceTableData["adapter"]    = item.at("adapter");
                    ifaceTableData["type"]       = item.at("type");
                    ifaceTableData["state"]      = item.at("state");
                    ifaceTableData["mtu"]        = item.at("mtu");
                    ifaceTableData["mac"]        = item.at("mac");
                    ifaceTableData["tx_packets"] = item.at("tx_packets");
                    ifaceTableData["rx_packets"] = item.at("rx_packets");
                    ifaceTableData["tx_errors"]  = item.at("tx_errors");
                    ifaceTableData["rx_errors"]  = item.at("rx_errors");
                    ifaceTableData["tx_dropped"] = item.at("tx_dropped");
                    ifaceTableData["rx_dropped"] = item.at("rx_dropped");
                    ifaceTableDataList.push_back(ifaceTableData);

                    // "dbsync_network_protocol" table data to update and notify
                    nlohmann::json protoTableData {};
                    protoTableData["iface"]   = item.at("name");
                    protoTableData["type"]    = item.at("type");
                    protoTableData["gateway"] = item.at("gateway");

                    if (item.find("IPv4") != item.end())
                    {
                        nlohmann::json addressTableData(item.at("IPv4"));
                        protoTableData["dhcp"]    = addressTableData.at("dhcp");
                        protoTableData["metric"]  = addressTableData.at("metric");

                        // "dbsync_network_address" table data to update and notify
                        addressTableData["iface"]   = item.at("name");
                        addressTableData["proto"]   = "IPv4";
                        addressTableDataList.push_back(addressTableData);
                    }

                    if (item.find("IPv6") != item.end())
                    {
                        nlohmann::json addressTableData(item.at("IPv6"));
                        protoTableData["dhcp"]    = addressTableData.at("dhcp");
                        protoTableData["metric"]  = addressTableData.at("metric");

                        // "dbsync_network_address" table data to update and notify
                        addressTableData["iface"] = item.at("name");
                        addressTableData["proto"] = "IPv6";
                        addressTableDataList.push_back(addressTableData);
                    }
                    protoTableDataList.push_back(protoTableData);
                }

                updateAndNotifyChanges(m_spDBSync->handle(), netIfaceTable,    ifaceTableDataList, m_reportFunction);
                updateAndNotifyChanges(m_spDBSync->handle(), netProtocolTable, protoTableDataList, m_reportFunction);
                updateAndNotifyChanges(m_spDBSync->handle(), netAddressTable,  addressTableDataList, m_reportFunction);
                m_spRsync->startSync(m_spDBSync->handle(), nlohmann::json::parse(NETIFACE_START_CONFIG_STATEMENT), m_reportFunction);
                m_spRsync->startSync(m_spDBSync->handle(), nlohmann::json::parse(NETPROTO_START_CONFIG_STATEMENT), m_reportFunction);
                m_spRsync->startSync(m_spDBSync->handle(), nlohmann::json::parse(NETADDRESS_START_CONFIG_STATEMENT), m_reportFunction);
            }
        }
    }
}

void Syscollector::scanPackages()
{
    if (m_packages)
    {
        constexpr auto tablePackages{"dbsync_packages"};
        nlohmann::json packages;
        nlohmann::json hotfixes;
        const auto& packagesData { m_spInfo->packages() };

        if (!packagesData.is_null())
        {
            for (const auto& item : packagesData)
            {
                if(item.find("hotfix") != item.end())
                {
                    if (m_hotfixes)
                    {
                        hotfixes.push_back(item);
                    }
                }
                else
                {
                    packages.push_back(item);
                }
            }
            updateAndNotifyChanges(m_spDBSync->handle(), tablePackages, packages, m_reportFunction);
            m_spRsync->startSync(m_spDBSync->handle(), nlohmann::json::parse(PACKAGES_START_CONFIG_STATEMENT), m_reportFunction);
            if (m_hotfixes)
            {
                constexpr auto tableHotfixes{"dbsync_hotfixes"};
                updateAndNotifyChanges(m_spDBSync->handle(), tableHotfixes, hotfixes, m_reportFunction);
                m_spRsync->startSync(m_spDBSync->handle(), nlohmann::json::parse(HOTFIXES_START_CONFIG_STATEMENT), m_reportFunction);
            }
        }
    }
}

void Syscollector::scanPorts()
{
    if (m_ports)
    {
        constexpr auto table{"dbsync_ports"};
        constexpr auto PORT_LISTENING_STATE { "listening" };
        constexpr auto TCP_PROTOCOL { "tcp" };
        const auto& data { m_spInfo->ports() };
        nlohmann::json portsList{};

        if (!data.is_null())
        {
            const auto& itPorts { data.find("ports") };

            if (data.end() != itPorts)
            {
                for (const auto& item : itPorts.value())
                {
                    const auto isListeningState { item.at("state") == PORT_LISTENING_STATE };
                    if(isListeningState)
                    {
                        // Only update and notify "Listening" state ports
                        if (m_portsAll)
                        {
                            // TCP and UDP ports
                            portsList.push_back(item);
                        }
                        else
                        {
                            // Only TCP ports
                            const auto isTCPProto { item.at("protocol") == TCP_PROTOCOL };
                            if (isTCPProto)
                            {
                                portsList.push_back(item);
                            }
                        }
                    }
                }
                updateAndNotifyChanges(m_spDBSync->handle(), table, portsList, m_reportFunction);
                m_spRsync->startSync(m_spDBSync->handle(), nlohmann::json::parse(PORTS_START_CONFIG_STATEMENT), m_reportFunction);
            }
        }
    }
}

void Syscollector::scanProcesses()
{
    if (m_processes)
    {
        constexpr auto table{"dbsync_processes"};
        const auto& processes{m_spInfo->processes()};
        if (!processes.is_null())
        {
            updateAndNotifyChanges(m_spDBSync->handle(), table, processes, m_reportFunction);
            m_spRsync->startSync(m_spDBSync->handle(), nlohmann::json::parse(PROCESSES_START_CONFIG_STATEMENT), m_reportFunction);
        }
    }
}

void Syscollector::scan()
{
    scanHardware();
    scanOs();
    scanNetwork();
    scanPackages();
    scanPorts();
    scanProcesses();
}

void Syscollector::syncLoop()
{
    if (m_scanOnStart)
    {
        scan();
    }
    while(sleepFor())
    {
        scan();
        //sync Rsync
    }
    m_spRsync.reset(nullptr);
    m_spDBSync.reset(nullptr);
}