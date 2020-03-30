import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { TranslateModule } from '@ngx-translate/core';

import {
  PageNotFoundComponent,
  FlowChartComponent,
  PressureChartComponent,
  VolumeChartComponent
} from './components/';
import { WebviewDirective } from './directives/';
import { FormsModule } from '@angular/forms';

const components = [
  PageNotFoundComponent,
  FlowChartComponent,
  PressureChartComponent,
  VolumeChartComponent
];

@NgModule({
  imports: [CommonModule, TranslateModule, FormsModule],
  declarations: [...components, WebviewDirective],
  exports: [components]
})
export class SharedModule {}
