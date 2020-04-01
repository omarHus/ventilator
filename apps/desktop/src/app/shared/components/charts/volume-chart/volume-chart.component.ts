import { Component } from '@angular/core';
import { BaseChartComponent } from '../base-chart.component';
import { HealthService } from '../../../../core/services';
import { HealtDataTypeEnum } from '../../../../core/services/health';

@Component({
  selector: 'app-volume-chart',
  templateUrl: './volume-chart.component.html',
  styleUrls: ['../base-chart.component.scss'],
  moduleId: module.id
})
export class VolumeChartComponent extends BaseChartComponent {
  constructor(_healthService: HealthService) {
    super(_healthService, HealtDataTypeEnum.volume);
  }
}
