import { HealthData } from "./health.class";

export interface IHealthData {
  timestamp: number;
  value: number;
}

export interface IVentilatorData {
  circuitId: string;
  flow: HealthData;
  volume: HealthData;
  pressure: HealthData;
}

export interface IVentilatorDataset {
  [key: string]: IVentilatorData;
}