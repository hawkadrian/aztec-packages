import type { Database, Key } from 'lmdb';

import type { AztecArray, AztecAsyncArray } from '../interfaces/array.js';
import type { Value } from '../interfaces/common.js';
import { LmdbAztecSingleton } from './singleton.js';

/** The shape of a key that stores a value in an array */
type ArrayIndexSlot = ['array', string, 'slot', number];

/**
 * An persistent array backed by LMDB.
 */
export class LmdbAztecArray<T extends Value> implements AztecArray<T>, AztecAsyncArray<T> {
  #db: Database<T, ArrayIndexSlot>;
  #name: string;
  #length: LmdbAztecSingleton<number>;

  constructor(db: Database<unknown, Key>, arrName: string) {
    this.#name = arrName;
    this.#length = new LmdbAztecSingleton(db, `${arrName}:meta:length`);
    this.#db = db as Database<T, ArrayIndexSlot>;
  }

  get length(): number {
    return this.#length.get() ?? 0;
  }

  lengthAsync(): Promise<number> {
    return Promise.resolve(this.length);
  }

  push(...vals: T[]): Promise<number> {
    return this.#db.childTransaction(() => {
      let length = this.length;
      for (const val of vals) {
        void this.#db.put(this.#slot(length), val);
        length += 1;
      }

      void this.#length.set(length);

      return length;
    });
  }

  pop(): Promise<T | undefined> {
    return this.#db.childTransaction(() => {
      const length = this.length;
      if (length === 0) {
        return undefined;
      }

      const slot = this.#slot(length - 1);
      const val = this.#db.get(slot) as T;

      void this.#db.remove(slot);
      void this.#length.set(length - 1);

      return val;
    });
  }

  at(index: number): T | undefined {
    if (index < 0) {
      index = this.length + index;
    }

    // the Array API only accepts indexes in the range [-this.length, this.length)
    // so if after normalizing the index is still out of range, return undefined
    if (index < 0 || index >= this.length) {
      return undefined;
    }

    return this.#db.get(this.#slot(index));
  }

  atAsync(index: number): Promise<T | undefined> {
    return Promise.resolve(this.at(index));
  }

  setAt(index: number, val: T): Promise<boolean> {
    if (index < 0) {
      index = this.length + index;
    }

    if (index < 0 || index >= this.length) {
      return Promise.resolve(false);
    }

    return this.#db.put(this.#slot(index), val);
  }

  *entries(): IterableIterator<[number, T]> {
    const transaction = this.#db.useReadTransaction();
    try {
      const values = this.#db.getRange({
        start: this.#slot(0),
        limit: this.length,
        transaction,
      });

      for (const { key, value } of values) {
        const index = key[3];
        yield [index, value];
      }
    } finally {
      transaction.done();
    }
  }

  async *entriesAsync(): AsyncIterableIterator<[number, T]> {
    for (const [key, value] of this.entries()) {
      yield [key, value];
    }
  }

  *values(): IterableIterator<T> {
    for (const [_, value] of this.entries()) {
      yield value;
    }
  }

  async *valuesAsync(): AsyncIterableIterator<T> {
    for (const [_, value] of this.entries()) {
      yield value;
    }
  }

  [Symbol.iterator](): IterableIterator<T> {
    return this.values();
  }

  [Symbol.asyncIterator](): AsyncIterableIterator<T> {
    return this.valuesAsync();
  }

  #slot(index: number): ArrayIndexSlot {
    return ['array', this.#name, 'slot', index];
  }
}
