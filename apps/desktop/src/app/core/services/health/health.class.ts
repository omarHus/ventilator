import { HealtDataTypeEnum } from "./health.enum";
import { IHealthData } from "./health.interface";
import { Queue } from "./queue.class";

export class HealthData {
  private _dataset: Queue<IHealthData>;
  
  public get data(): number[] {
    return this._dataset.queue.map((item: IHealthData) => item.value);
  }
  
  public get time(): string[] {
    return this._dataset.queue.map((item: IHealthData) => item.timestamp.toFixed(1));
  }

  constructor(
    public dataType: HealtDataTypeEnum,
    public capacity: number
  ) {
    this._dataset = new Queue();
  }

  public add(value: IHealthData): void {
    if (this._dataset.size() >= this.capacity) {
      this._dataset.remove();      
    }

    this._dataset.add(value);
  }
}