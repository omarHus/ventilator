import { Injectable } from '@angular/core';
import { Subject, timer } from 'rxjs';

import { IVentilatorDataset } from './health.interface';

@Injectable({
  providedIn: 'root'
})
export class HealthService {
    public onHealthUpdate: Subject<IVentilatorDataset> = new Subject<IVentilatorDataset>();

    public mockFlow = [0, 2, 4, 8, 16, 8, 4, 2, 1];
    public mockVolume = [0, 300, 400, 500, 400, 300, 200, 100, 50, 0];
    public mockPressure = [25, 25, 22, 20, 18, 15, 12, 9, 5, 5];

    private _count = 0;
 
    constructor() {
        timer(0, 100).subscribe((sec: number) => {
            const dataset: IVentilatorDataset = {
                '1': {
                    timestamp: sec,
                    circuitId: '1',
                    flow: this.mockFlow[this._count],
                    volume: this.mockVolume[this._count],
                    pressure: this.mockPressure[this._count]
                },
                '2': {
                    timestamp: sec,
                    circuitId: '2',
                    flow: this.mockFlow[this._count],
                    volume: this.mockVolume[this._count],
                    pressure: this.mockPressure[this._count]
                },
            };
            this._count++;
            if (this._count >= this.mockFlow.length-1) { this._count = 0; }

            this.onHealthUpdate.next(dataset);
        })
    }
}
