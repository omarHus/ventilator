import { Component } from '@angular/core';
import { BaseChartComponent } from '../base-chart.component';
import { HealthService } from '../../../../core/services';
import { HealtDataTypeEnum } from '../../../../core/services/health';

@Component({
  selector: 'app-pressure-chart',
  templateUrl: './pressure-chart.component.html',
  styleUrls: ['../base-chart.component.scss'],
  moduleId: module.id
})
export class PressureChartComponent extends BaseChartComponent {
  constructor(_healthService: HealthService) {
    super(_healthService, HealtDataTypeEnum.pressure);
  }
}
