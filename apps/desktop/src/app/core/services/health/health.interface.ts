export interface IHealthData {
  timestamp: number;
  value: number;
}

export interface IVentilatorData {
  timestamp: number;
  circuitId: string;
  flow: number;
  volume: number;
  pressure: number;
}

export interface IVentilatorDataset {
  [key: string]: IVentilatorData;
}