
export function tuv(arr: number[]) {
  const result = [...arr];
  for (let v = 7; v < result.length; v += 8) {
    result[v] = 1 - result[v]
  }

  return result;
}