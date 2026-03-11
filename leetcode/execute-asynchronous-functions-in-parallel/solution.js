/**
 * @param {number[]} nums
 * @return {void}
 */
var ArrayWrapper = function(nums) {
    this.nums = nums;
};

/**
 * @return {number}
 */
ArrayWrapper.prototype.valueOf = function() {
    return this.nums.reduce((sum, x) => sum + x, 0);
}

/**
 * @return {string}
 */
ArrayWrapper.prototype.toString = function() {
    return `[${this.nums.join(",")}]`;
}

/**
 * const obj1 = new ArrayWrapper([1,2]);
 * const obj2 = new ArrayWrapper([3,4]);
 * obj1 + obj2; // 10
 * String(obj1); // "[1,2]"
 * String(obj2); // "[3,4]"
 */

/**
 * @param {Function[]} functions
 * @return {Promise<any[]>}
 */
var promiseAll = function(functions) {
    return new Promise((resolve, reject) => {
        const n = functions.length;
        if (n === 0) {
            resolve([]);
            return;
        }

        const results = new Array(n);
        let resolvedCount = 0;
        let rejected = false;

        functions.forEach((fn, index) => {
            Promise.resolve()
                .then(() => fn())
                .then((value) => {
                    if (rejected) return;
                    results[index] = value;
                    resolvedCount += 1;
                    if (resolvedCount === n) {
                        resolve(results);
                    }
                })
                .catch((error) => {
                    if (rejected) return;
                    rejected = true;
                    reject(error);
                });
        });
    });
};
