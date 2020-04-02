import { Injectable } from '@angular/core';
import * as SerialPort from 'serialport';
import * as ReadLine from '@serialport/parser-readline';

@Injectable({
  providedIn: 'root'
})
export class SerialPortService {
  public isOpen: boolean = false;

  private _port: SerialPort;
  public get port(): SerialPort {
    if (!this._port) {
      // TODO: read this value from the enviroment file for the url and port number
      this.initPort('/dev/cu.usbmodem1411', 9600);
    }
    return this._port;
  }

  public parser: any;

  constructor() {
    // TODO: read this value from the enviroment file for the url and port number
    this.initPort('/dev/cu.usbmodem1411', 9600);
  }

  private initPort(path: string, baudRate: number): void {
    this._port = new SerialPort(path, { baudRate });
    this.parser = this._port.pipe(new ReadLine({delimiter:'\n'}));
  }
}