#include "serialportwidget.h"
#include "ui_SerialPortWidget.h"

// TODO: One single button for Connect/Disconnect

SerialPortWidget::SerialPortWidget ( QWidget *parent ) :
        QWidget( parent ), ui( new Ui::SerialPortWidget ) {
    ui->setupUi( this );

    // Disable maximizing
    QWidget::setFixedSize( width( ), height( ));

    // Adding title for widget
    QWidget::setWindowTitle( "Serial Port" );

    /* Add ports to serialPortsBox */
    refreshComPortList( );

    /* Add baud rates to baudRateBox */
    QList < qint32 > baudRatesAvailable = QSerialPortInfo::standardBaudRates( );
    for ( auto &baudRateIterator: baudRatesAvailable ) {
        ui->baudRateBox->addItem( QString::number( baudRateIterator ), baudRateIterator );
    }
    ui->baudRateBox->setCurrentIndex( 6 );

    /* Data bits */
    ui->dataBitsBox->addItems( { "5 Bits", "6 Bits", "7 Bits", "8 Bits" } );
    ui->dataBitsBox->setCurrentText( "8 Bits" );

    /* Stop bits */
    ui->stopBitsBox->addItems( { "1 Bit", "2 Bits", "3 Bits" } );
    ui->stopBitsBox->setCurrentText( "1 Bit" );

    /* Parity */
    ui->parityBox->addItems( { "No Parity", "Even Parity", "Odd Parity", "Space Parity", "Mark Parity" } );
    ui->parityBox->setCurrentText( "No Parity" );

    /* Flow control */
    ui->flowControlBox->addItems( { "No Flow Control", "Hardware Flow Control", "Software Flow Control" } );
    ui->flowControlBox->setCurrentText( "No Flow Control" );

    connect( ui->refreshPortsButton, &QPushButton::released, this, &SerialPortWidget::refreshComPortList );
    connect( ui->connectButton, &QPushButton::released, this, &SerialPortWidget::connectButton );

    connect( ui->disconnectButton, &QPushButton::released, this, &SerialPortWidget::disconnectButton );
    connect( ui->sendAction, &QPushButton::released, this, &SerialPortWidget::sendAction );
    connect( ui->writeFileCheckbox, &QPushButton::released, this, &SerialPortWidget::writeFileCheckboxHandle );
    connect( ui->readFileCheckbox, &QPushButton::released, this, &SerialPortWidget::readFileCheckboxHandle );
    connect( ui->updateFileCheckbox, &QPushButton::released, this, &SerialPortWidget::updateFileCheckboxHandle );
    connect( ui->deleteFileCheckbox, &QPushButton::released, this, &SerialPortWidget::deleteFileCheckboxHandle );

}

SerialPortWidget::~SerialPortWidget ( ) noexcept {
    if ( serialPort.isOpen( ))
        serialPort.close( );
    delete ui;
}

void SerialPortWidget::messageReceived ( ) {

    if ( serialPort.isOpen( )) {

        const QByteArray byteArray = serialPort.readAll( );
        bufferReceiver = byteArray;
        MSGPack::Pack( receiveMSGPack, bufferReceiver.toStdString( ));

        ui->messageReceived->clear( );
        if ( receiveMSGPack.getCommand( )[0] == PRINT ) {

            int j = 0;
            int size = 0;
            int element = 0;
            std::set < std::string > driveContent;
            std::string msg = receiveMSGPack.getContent( );
            QString content;

            for ( int i = 0; i < msg.length( ); ++i ) {
                if ( msg[i] == ';' ) {
                    size = i - j;
                    std::string pathFile;
                    pathFile = msg.substr( j, size );
                    driveContent.insert( pathFile );
                    ++i;
                    j = i;
                    ++element;
                }
            }

            for ( auto &data: driveContent ) {
                content.append( data.c_str( ));
                content.append( "\n" );
            }

            ui->deviceInfoText->clear( );
            ui->deviceInfoText->setText( content );

        } else if ( receiveMSGPack.getCommand( )[0] == ERROR ) {

            ui->messageReceived->append( "Error!" );

        } else if ( receiveMSGPack.getCommand( )[0] == READ ) {

            ui->messageReceived->append( receiveMSGPack.getContent( ).c_str( ));

        } else {

            ui->messageReceived->append( "Success!" );

        }
    }
}

void SerialPortWidget::connectButton ( ) {

    // Set Serial Port Name
    setSerialPortName( );
    // Set Baud Rate
    setBaudRate( );
    // Set Data Bits
    setDataBits( );
    // Set Stop Bits
    setStopBits( );
    // Set Parity Bits
    setParity( );
    // Set Flow Control
    setFlowControl( );

    serialPort.open( QIODevice::ReadWrite );

    connect( &serialPort, SIGNAL( readyRead( )), this, SLOT( messageReceived( )));

    if ( serialPort.isOpen( )) {
        std::string command;
        command.push_back( PRINT );
        std::string fileName;
        std::string content;
        const MSGPack msgPack = { command, fileName, content };
        setReceiveMSGPack( msgPack );
        requestDeviceInfo( receiveMSGPack );
    }

}

void SerialPortWidget::disconnectButton ( ) { serialPort.close( ); }

void SerialPortWidget::refreshComPortList ( ) {

    //TODO: If there are the same do not add them again
    QList < QSerialPortInfo > serialPortsAvailable = QSerialPortInfo::availablePorts( );
    ui->serialPortsBox->clear( );
    for ( auto &portIterator: serialPortsAvailable ) {
        ui->serialPortsBox->addItem( portIterator.portName( ));
    }

}

void SerialPortWidget::requestDeviceInfo ( MSGPack &data ) {

    std::string json;
    MSGPack::PackToString( data, json );

    serialPort.write( json.c_str( ));

}

void SerialPortWidget::sendAction ( ) {

    std::string fileName = ui->fileNameTextField->toPlainText( ).toStdString( );
    std::string content = ui->contentTextField->toPlainText( ).toStdString( );

    std::string command;
    if ( ui->writeFileCheckbox->isChecked( )) {
        command.push_back( WRITE );
    } else if ( ui->readFileCheckbox->isChecked( )) {
        command.push_back( READ );
    } else if ( ui->updateFileCheckbox->isChecked( )) {
        command.push_back( UPDATE );
    } else if ( ui->deleteFileCheckbox->isChecked( )) {
        command.push_back( DELETE );
    }

    std::string json;
    const MSGPack msgPack( command, fileName, content );
    MSGPack::PackToString( msgPack, json );

    serialPort.write( json.c_str( ));

}

void SerialPortWidget::writeFileCheckboxHandle ( ) {

    if ( ui->writeFileCheckbox->isChecked( )) {

        if ( ui->readFileCheckbox->isChecked( ))
            ui->readFileCheckbox->toggle( );
        if ( ui->updateFileCheckbox->isChecked( ))
            ui->updateFileCheckbox->toggle( );
        if ( ui->deleteFileCheckbox->isChecked( ))
            ui->deleteFileCheckbox->toggle( );

        ui->contentTextField->clear( );
        ui->contentTextField->setEnabled( true );

    }

}

void SerialPortWidget::readFileCheckboxHandle ( ) {

    if ( ui->readFileCheckbox->isChecked( )) {

        if ( ui->writeFileCheckbox->isChecked( ))
            ui->writeFileCheckbox->toggle( );
        if ( ui->updateFileCheckbox->isChecked( ))
            ui->updateFileCheckbox->toggle( );
        if ( ui->deleteFileCheckbox->isChecked( ))
            ui->deleteFileCheckbox->toggle( );

        ui->contentTextField->clear( );
        ui->contentTextField->setEnabled( false );

    }

}

void SerialPortWidget::updateFileCheckboxHandle ( ) {

    if ( ui->updateFileCheckbox->isChecked( )) {

        if ( ui->writeFileCheckbox->isChecked( ))
            ui->writeFileCheckbox->toggle( );
        if ( ui->readFileCheckbox->isChecked( ))
            ui->readFileCheckbox->toggle( );
        if ( ui->deleteFileCheckbox->isChecked( ))
            ui->deleteFileCheckbox->toggle( );

        ui->contentTextField->clear( );
        ui->contentTextField->setEnabled( true );

    }

}

void SerialPortWidget::deleteFileCheckboxHandle ( ) {

    if ( ui->deleteFileCheckbox->isChecked( )) {

        if ( ui->writeFileCheckbox->isChecked( ))
            ui->writeFileCheckbox->toggle( );
        if ( ui->readFileCheckbox->isChecked( ))
            ui->readFileCheckbox->toggle( );
        if ( ui->updateFileCheckbox->isChecked( ))
            ui->updateFileCheckbox->toggle( );

        ui->contentTextField->clear( );
        ui->contentTextField->setEnabled( false );

    }

}

void SerialPortWidget::setDataBits ( ) {

    QString dataBits = ui->dataBitsBox->currentText( );
    if ( dataBits == "5 Bits" ) {
        serialPort.setDataBits( QSerialPort::Data5 );
    } else if (( dataBits == "6 Bits" )) {
        serialPort.setDataBits( QSerialPort::Data6 );
    } else if ( dataBits == "7 Bits" ) {
        serialPort.setDataBits( QSerialPort::Data7 );
    } else if ( dataBits == "8 Bits" ) {
        serialPort.setDataBits( QSerialPort::Data8 );
    }

}

void SerialPortWidget::setSerialPortName ( ) {

    QString portName = ui->serialPortsBox->currentText( );
    serialPort.setPortName( portName );

}

void SerialPortWidget::setBaudRate ( ) {

    QString baudRate = ui->baudRateBox->currentText( );
    serialPort.setBaudRate( baudRate.toInt( ));

}

void SerialPortWidget::setStopBits ( ) {

    QString stopBits = ui->stopBitsBox->currentText( );
    if ( stopBits == "1 Bit" ) {
        serialPort.setStopBits( QSerialPort::OneStop );
    } else if ( stopBits == "1,5 Bits" ) {
        serialPort.setStopBits( QSerialPort::OneAndHalfStop );
    } else if ( stopBits == "2 Bits" ) {
        serialPort.setStopBits( QSerialPort::TwoStop );
    }

}

void SerialPortWidget::setParity ( ) {

    QString parity = ui->parityBox->currentText( );
    if ( parity == "No Parity" ) {
        serialPort.setParity( QSerialPort::NoParity );
    } else if ( parity == "Even Parity" ) {
        serialPort.setParity( QSerialPort::EvenParity );
    } else if ( parity == "Odd Parity" ) {
        serialPort.setParity( QSerialPort::OddParity );
    } else if ( parity == "Mark Parity" ) {
        serialPort.setParity( QSerialPort::MarkParity );
    } else if ( parity == "Space Parity" ) {
        serialPort.setParity( QSerialPort::SpaceParity );
    }

}

void SerialPortWidget::setFlowControl ( ) {

    QString flowControl = ui->flowControlBox->currentText( );
    if ( flowControl == "No Flow Control" ) {
        serialPort.setFlowControl( QSerialPort::NoFlowControl );
    } else if ( flowControl == "Hardware Flow Control" ) {
        serialPort.setFlowControl( QSerialPort::HardwareControl );
    } else if ( flowControl == "Software Flow Control" ) {
        serialPort.setFlowControl( QSerialPort::SoftwareControl );
    }

}
