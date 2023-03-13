
const stack = [];
const out = [];

const TO = 136;

for (let i = 0; i < TO; ++i) {
    const entry = [i, 0];
    out.push(entry);
    stack.push(entry);

    while (stack.length > 1) {
        if (stack[stack.length - 1][1] >= stack[stack.length - 2][1]) {
            const b = stack.pop();
            const a = stack.pop();
            const c = [a[0], a[1] + 1];
            out.push(c);
            stack.push(c);
        } else {
            break;
        }
    }
}

while (stack.length > 1) {
    const b = stack.pop();
    const a = stack.pop();
    const c = [a[0], a[1] + 1];
    out.push(c);
    stack.push(c);
}

for (let i = 0; i < out.length; ++i) {
    const idx  = i;
    const lvl  = out[i][1];
    const from = out[i][0];
    const to   = Math.min(from + (1 << lvl) - 1, TO);
    const predictedIdx = indexOf(lvl, from);
    if (lvl === 0) {
        console.log(`${idx}\t${from}\t${lvl}\t${predictedIdx}\t${idx !== predictedIdx ? 'WRONG' : ''}`);
    } else {
        console.log(`${idx}\t${from}-${to}\t${lvl}\t${predictedIdx}\t${idx !== predictedIdx ? `WRONG\t${idx - predictedIdx}` : ''}`);
    }
}

function indexOf(lvl, from) {
    if (lvl === 0) {
        return sumBits(from);
    } else {
        const next = from + (1 << lvl);
        return sumBits(next) - countBits(from) + countBits(next) - 2;
    }
}

function sumBits(val) {
    let sum = 0;
    while (val > 0) {
        sum += val;
        val >>>= 1;
    }
    return sum;
}

function countBits(val) {
    let count = 0;
    while (val > 0) {
        if (val & 1) ++count;
        val >>>= 1;
    }
    return count;
}