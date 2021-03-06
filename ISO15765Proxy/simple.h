#pragma once

#ifndef _SIMPLE_H
#define _SIMPLE_H

#include "ISO15765Proxy.h"
#include <exception>
#include <list>
#include "internal.h"
#include <memory>

DEFINE_SHARED(PeriodicMessageSimple)
DEFINE_SHARED(MessageFilterSimple)
DEFINE_SHARED(ChannelSimple)
DEFINE_SHARED(DeviceSimple)
DEFINE_SHARED(LibrarySimple)

class J2534Exception : public std::exception {
public:
    J2534Exception(long code);

    long code() const;

    virtual const char *what() const noexcept;

private:
    long mCode;
};

class LibrarySimple : public Library {
public:
    LibrarySimple(j2534_fcts *proxy);

    virtual ~LibrarySimple();

    virtual DevicePtr open(void *pName) override;

    virtual void close(const DevicePtr &devicePtr) override;

    virtual void getLastError(char *pErrorDescription) override;
    
    // New

    virtual j2534_fcts* getProxy() const;
    
protected:
    virtual DevicePtr createDevice(void *pName, unsigned long deviceId);

    std::list<DevicePtr> mDevices;
    j2534_fcts *mProxy;
};

class DeviceSimple : public Device {
    friend class LibrarySimple;
public:
    DeviceSimple(const LibrarySimplePtr &library, unsigned long deviceId);

    virtual ~DeviceSimple();

    virtual ChannelPtr connect(unsigned long ProtocolID, unsigned long Flags, unsigned long BaudRate) override;

    virtual void disconnect(const ChannelPtr &channelPtr) override;

    virtual void setProgrammingVoltage(unsigned long PinNumber, unsigned long Voltage) override;

    virtual void readVersion(char *pFirmwareVersion, char *pDllVersion, char *pApiVersion) override;

    virtual void ioctl(unsigned long IoctlID, void *pInput, void *pOutput) override;

    virtual LibraryWeakPtr getLibrary() const override;
    
protected:
    virtual ChannelPtr createChannel(unsigned long ProtocolID, unsigned long Flags, unsigned long BaudRate, unsigned long channelId);

    LibrarySimpleWeakPtr mLibrary;
    std::list<ChannelPtr> mChannels;
    unsigned long mDeviceId;
};

class ChannelSimple : public Channel {
    friend class DeviceSimple;
public:
    ChannelSimple(const DeviceSimplePtr &device, unsigned long channelId);

    virtual ~ChannelSimple();

    virtual void readMsgs(PASSTHRU_MSG *pMsg, unsigned long *pNumMsgs, unsigned long Timeout) override;

    virtual void writeMsgs(PASSTHRU_MSG *pMsg, unsigned long *pNumMsgs, unsigned long Timeout) override;

    virtual PeriodicMessagePtr startPeriodicMsg(PASSTHRU_MSG *pMsg, unsigned long TimeInterval) override;

    virtual void stopPeriodicMsg(const PeriodicMessagePtr &periodicMessage) override;

    virtual MessageFilterPtr startMsgFilter(unsigned long FilterType, PASSTHRU_MSG *pMaskMsg, PASSTHRU_MSG *pPatternMsg,
                                         PASSTHRU_MSG *pFlowControlMsg) override;

    virtual void stopMsgFilter(const MessageFilterPtr &messageFilter) override;

    virtual void ioctl(unsigned long IoctlID, void *pInput, void *pOutput) override;

    virtual DeviceWeakPtr getDevice() const override;
    
protected:
    virtual MessageFilterPtr createMessageFilter(unsigned long FilterType, PASSTHRU_MSG *pMaskMsg, PASSTHRU_MSG *pPatternMsg, PASSTHRU_MSG *pFlowControlMsg, unsigned long messageFilterId);
    virtual PeriodicMessagePtr createPeriodicMessage(PASSTHRU_MSG *pMsg, unsigned long TimeInterval, unsigned long periodicMessageId);

    DeviceSimpleWeakPtr mDevice;
    std::list<MessageFilterPtr> mMessageFilters;
    std::list<PeriodicMessagePtr> mPeriodicMessages;
    unsigned long mChannelId;
};

class MessageFilterSimple : public MessageFilter {
    friend class ChannelSimple;
public:
    MessageFilterSimple(const ChannelSimplePtr& channel, unsigned long messageFilterId);
    ~MessageFilterSimple();

    virtual ChannelWeakPtr getChannel() const override;
protected:
    ChannelSimpleWeakPtr mChannel;
    unsigned long mMessageFilterId;
};

class PeriodicMessageSimple : public PeriodicMessage {
    friend class ChannelSimple;
public:
    PeriodicMessageSimple(const ChannelSimplePtr& channel, unsigned long periodicMessageId);
    ~PeriodicMessageSimple();

    virtual ChannelWeakPtr getChannel() const override;
protected:
    ChannelSimpleWeakPtr mChannel;
    unsigned long mPeriodicMessageId;
};

#endif //_SIMPLE_H
