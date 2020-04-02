import { Injectable } from '@angular/core';
import * as SerialPort from 'serialport';

@Injectable({
  providedIn: 'root'
})
export class SerialPortService {
  public isOpen: boolean = false;

  private _port: SerialPort;
  public get port(): SerialPort {
    if (!this._port) {
      // TODO: read this value from the enviroment file for the url and port number
      this.initPort('/dev/cu.usbmodem14101', 9600);
    }
    return this._port;
  }

  constructor() {
    // TODO: read this value from the enviroment file for the url and port number
    this.initPort('/dev/cu.usbmodem14101', 9600);
  }

  private initPort(path: string, baudRate: number): void {
    this._port = new SerialPort(path, { baudRate });
  }
}