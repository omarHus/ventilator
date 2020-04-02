import { Injectable } from '@angular/core';
import { Subject, timer } from 'rxjs';

import { IVentilatorDataset } from './health.interface';
import { HealthData } from './health.class';
import { HealtDataTypeEnum } from './health.enum';

@Injectable({
  providedIn: 'root'
})
export class HealthService {
    public onHealthUpdate: Subject<void> = new Subject<void>();
    
    public dataset: IVentilatorDataset;

    public volumeP1 = new HealthData(HealtDataTypeEnum.volume, 18);
    public flowP1 = new HealthData(HealtDataTypeEnum.flow, 18);
    public pressureP1 = new HealthData(HealtDataTypeEnum.pressure, 18);

    public volumeP2 = new HealthData(HealtDataTypeEnum.volume, 18);
    public flowP2 = new HealthData(HealtDataTypeEnum.flow, 18);
    public pressureP2 = new HealthData(HealtDataTypeEnum.pressure, 18);


    public mockFlow = [0, 2, 4, 8, 16, 8, 4, 2, 1];
    public mockVolume = [0, 300, 400, 500, 400, 300, 200, 100, 50, 0];
    public mockPressure = [25, 25, 22, 20, 18, 15, 12, 9, 5, 5];

    private _count = 0;
    private _transmissionStartTime = new Date().getTime();
    constructor() {
      this.dataset = this.initDataset();

        timer(0, 100).subscribe((sec: number) => {
            const timestamp = (new Date().getTime() - this._transmissionStartTime)/1000;
            
            this.addData('p1', HealtDataTypeEnum.volume, timestamp, this.mockVolume[this._count]);
            this.addData('p1', HealtDataTypeEnum.flow, timestamp, this.mockFlow[this._count]);
            this.addData('p1', HealtDataTypeEnum.pressure, timestamp, this.mockPressure[this._count]);

            this.addData('p2', HealtDataTypeEnum.volume, timestamp, this.mockVolume[this._count]);
            this.addData('p2', HealtDataTypeEnum.flow, timestamp, this.mockFlow[this._count]);
            this.addData('p2', HealtDataTypeEnum.pressure, timestamp, this.mockPressure[this._count]);

            this._count++;
            if (this._count >= this.mockFlow.length-1) { this._count = 0; }

            this.onHealthUpdate.next();
        })
    }

    public addData(circuitId: string, dataType: HealtDataTypeEnum, timestamp: number, value: number): void {
      this.dataset[circuitId][dataType].add({ timestamp, value });
    }

    private initDataset(): IVentilatorDataset {
      return {
        'p1': {
          circuitId: 'p1',
          [HealtDataTypeEnum.volume]: new HealthData(HealtDataTypeEnum.volume, 18),
          [HealtDataTypeEnum.flow]: new HealthData(HealtDataTypeEnum.flow, 18),
          [HealtDataTypeEnum.pressure]: new HealthData(HealtDataTypeEnum.pressure, 18)
        },
        'p2': {
          circuitId: 'p2',
          [HealtDataTypeEnum.volume]: new HealthData(HealtDataTypeEnum.volume, 18),
          [HealtDataTypeEnum.flow]: new HealthData(HealtDataTypeEnum.flow, 18),
          [HealtDataTypeEnum.pressure]: new HealthData(HealtDataTypeEnum.pressure, 18)
        }
      };
    }
}
