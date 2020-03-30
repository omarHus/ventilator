export class Queue<T> {
  private _queue: T[] = [];
  public get queue(): T[] {
    return this._queue;
  }
  
  public add(value: T): void {
    this._queue.push(value);
  }

  public remove(): T {
    return this._queue.shift();
  }

  public isEmpty(): boolean {
    return this._queue.length === 0;
  }

  public size(): number {
    return this._queue.length;
  }

  public peek(): T {
    return !this.isEmpty() ? this._queue[0] : undefined;
    // return this._dataset[this.size() - 1];
  }
}