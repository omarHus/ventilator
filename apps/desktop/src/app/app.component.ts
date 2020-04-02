import { Component, OnInit } from '@angular/core';
import { ElectronService, SerialPortService, HealthService, HealtDataTypeEnum } from './core/services';
import { TranslateService } from '@ngx-translate/core';
@Component({

  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent implements OnInit {
  private _transmissionStartTime: number;
  constructor(
    public electronService: ElectronService,
    private _serialPortService: SerialPortService,
    private _healthService: HealthService,
    private _translate: TranslateService
  ) {
    _translate.setDefaultLang('en');
    // left as reference
    // console.log('AppConfig', AppConfig);

    if (electronService.isElectron) {
      // left as reference
      // console.log(process.env);
      // console.log('Mode electron');
      // console.log('Electron ipcRenderer', electronService.ipcRenderer);
      // console.log('NodeJS childProcess', electronService.childProcess);
    } else {
      console.log('Mode web');
    }
  }

  public ngOnInit(): void {
    this._serialPortService.port.on('open', () => {
      this._serialPortService.isOpen = true;
      this._transmissionStartTime = new Date().getTime();
      this,this.subscribeToPortEvents();
    });
  }

  private subscribeToPortEvents(): void {
    this._serialPortService.port.on('data', (data: string) => {
      try {
        const message = JSON.parse(data);
        const timestamp = (new Date().getTime() - this._transmissionStartTime)/1000;

        this._healthService.addData('p1', HealtDataTypeEnum.volume, timestamp, message['volume_p1']);
        this._healthService.addData('p1', HealtDataTypeEnum.flow, timestamp, message['flow_p1']);
        this._healthService.addData('p1', HealtDataTypeEnum.pressure, timestamp, message['pressure_p1']);

        this._healthService.addData('p2', HealtDataTypeEnum.volume, timestamp, message['volume_p2']);
        this._healthService.addData('p2', HealtDataTypeEnum.flow, timestamp, message['flow_p2']);
        this._healthService.addData('p2', HealtDataTypeEnum.pressure, timestamp, message['pressure_p2']);

        this._healthService.onHealthUpdate.next();
      } catch(error) {

      }
    });
  }
}
