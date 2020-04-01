import { Component } from '@angular/core';
import { BaseChartComponent } from '../base-chart.component';
import { HealtDataTypeEnum, HealthService } from '../../../../core/services/health';

@Component({
  selector: 'app-flow-chart',
  templateUrl: './flow-chart.component.html',
  styleUrls: ['../base-chart.component.scss'],
  moduleId: module.id
})
export class FlowChartComponent extends BaseChartComponent {
  constructor(_healthService: HealthService) {
    super(_healthService, HealtDataTypeEnum.flow);
  }
}
