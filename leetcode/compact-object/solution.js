/**
 * @param {Object|Array} obj
 * @return {Object|Array}
 */
var compactObject = function(obj) {
    if (obj === null || typeof obj !== 'object') {
        // Primitive value – simply return it as-is
        return obj;
    }

    if (Array.isArray(obj)) {
        const res = [];
        for (const value of obj) {
            const compacted = compactObject(value);
            if (Boolean(compacted)) {
                res.push(compacted);
            }
        }
        return res;
    }

    const res = {};
    for (const [key, value] of Object.entries(obj)) {
        const compacted = compactObject(value);
        if (Boolean(compacted)) {
            res[key] = compacted;
        }
    }
    return res;
};
