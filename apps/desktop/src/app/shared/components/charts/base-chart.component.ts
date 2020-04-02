import { Component, Input, ViewChild, ElementRef, AfterViewInit, OnDestroy } from '@angular/core';
import { Chart } from 'chart.js';

import { HealthService } from '../../../core/services';
import { Subject } from 'rxjs';
import { takeUntil, filter, map } from 'rxjs/operators';
import { HealthData, HealtDataTypeEnum } from '../../../core/services/health';
import { IVentilatorDataset, IVentilatorData } from '../../../core/services/health/health.interface';

@Component({
  template: ``,
  moduleId: module.id
})
export class BaseChartComponent implements AfterViewInit, OnDestroy {
  public chart: Chart;

  @Input() public circuitId: string = `circuit${module.id}`;
  @Input() public chartId: string = `chart${module.id}`;
  @Input() public title: string = `title`;
    
  private _unbsubscriber: Subject<void> = new Subject<void>();
  @ViewChild('airflowChart') private _airflowChart: ElementRef;

  constructor(
    private _healthService: HealthService,
    private _dataType: HealtDataTypeEnum,
  ) { }

  public ngAfterViewInit(): void {
    if (this._airflowChart) {
      this.chart = this.initChart(this._airflowChart.nativeElement);
      this.subscribeToSubjects();
    }
  }

  public ngOnDestroy(): void {
    this._unbsubscriber.complete();
    this._unbsubscriber.next();
  }

  private initChart(canvas: HTMLCanvasElement): Chart {
    var seedData = {
      labels: this._healthService.dataset[this.circuitId][this._dataType].time,
      datasets: [{
        label: this.title,
        data: this._healthService.dataset[this.circuitId][this._dataType].data,
        pointRadius: 0,
        borderColor: '#ff4081',
        fill: false,
        lineTension: 0.5,
      }]
    };
       
    return new Chart(canvas.getContext('2d'), {
      type: 'line',
      data: seedData,
      options: {
        aspectRatio: 1.4,
        maintainAspectRatio: false,
        legend: {
          display: false
        },
        responsive: true,
        animation: {
            duration: 0,
        },
     }
    });
  }

  private subscribeToSubjects(): void {
    this._healthService
      .onHealthUpdate
      .pipe(
        takeUntil(this._unbsubscriber)
      )
      .subscribe(() => {
        this.chart.data.labels = this._healthService.dataset[this.circuitId][this._dataType].time;
        this.chart.data.datasets[0].data = this._healthService.dataset[this.circuitId][this._dataType].data;
        this.chart.update();
      });
  }
}
