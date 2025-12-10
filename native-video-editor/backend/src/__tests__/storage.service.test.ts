import os from 'os';
import path from 'path';
import { promises as fs } from 'fs';
import { StorageService } from '../services/storage.service.js';

describe('StorageService', () => {
  let tempDir: string;
  let service: StorageService;

  beforeEach(async () => {
    tempDir = await fs.mkdtemp(path.join(os.tmpdir(), 'storage-service-'));
    service = new StorageService(tempDir);
    await service.ensureUploadDir();
  });

  afterEach(async () => {
    await fs.rm(tempDir, { recursive: true, force: true });
  });

  it('ensures upload directory exists when missing', async () => {
    await fs.rm(tempDir, { recursive: true, force: true });

    await service.ensureUploadDir();

    await expect(fs.access(tempDir)).resolves.not.toThrow();
  });

  it('creates file paths and deletes files', async () => {
    const filename = 'test.txt';
    const filePath = service.getFilePath(filename);

    expect(filePath).toBe(path.join(tempDir, filename));
    await expect(service.fileExists(filename)).resolves.toBe(false);

    await fs.writeFile(filePath, 'payload');
    await expect(service.fileExists(filename)).resolves.toBe(true);

    await service.deleteFile(filename);
    await expect(service.fileExists(filename)).resolves.toBe(false);
  });
});
