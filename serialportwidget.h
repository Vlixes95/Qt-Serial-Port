#ifndef QTSERIALPORT_SERIALPORTWIDGET_H
#define QTSERIALPORT_SERIALPORTWIDGET_H

#include <QWidget>
#include <QtSerialPort/QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "MSGPack.h"
#include "set"

QT_BEGIN_NAMESPACE
namespace Ui { class SerialPortWidget; }
QT_END_NAMESPACE

class SerialPortWidget : public QWidget {
Q_OBJECT

public:
    explicit SerialPortWidget ( QWidget *parent = nullptr );

    ~SerialPortWidget ( ) override;

    void setReceiveMSGPack ( const MSGPack &msgPack ) { receiveMSGPack = msgPack; }

private slots:

    void messageReceived ( );

    void connectButton ( );

    void disconnectButton ( );

    void refreshComPortList ( );

private:
    Ui::SerialPortWidget *ui;
    QSerialPort serialPort;
    QSerialPortInfo serialPortInfo;
    QString bufferReceiver;
    MSGPack receiveMSGPack;
    MSGPack sendMSGPack;

    void setSerialPortName ( );

    void setBaudRate ( );

    void setDataBits ( );

    void setStopBits ( );

    void setParity ( );

    void setFlowControl ( );

    // TODO: Clean message box when sending
    void sendAction ( );

    void writeFileCheckboxHandle ( );

    void readFileCheckboxHandle ( );

    void updateFileCheckboxHandle ( );

    void deleteFileCheckboxHandle ( );

    void requestDeviceInfo ( MSGPack &data );

    // TODO: Implement
    void refreshDeviceInfo ( );

};


#endif //QTSERIALPORT_SERIALPORTWIDGET_H
