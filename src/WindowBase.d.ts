export {WindowBase}

declare class WindowBase {
  initialize({ width, height, title }: {width?: number, height?: number, title?: string}?)
  makeContextCurrent(): void
  shouldClose(): boolean
  close(): void
}
