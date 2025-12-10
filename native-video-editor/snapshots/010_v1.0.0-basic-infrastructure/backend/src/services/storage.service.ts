import fs from 'fs/promises';
import path from 'path';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

/**
 * Storage service for managing uploaded files
 */
export class StorageService {
  private uploadDir: string;

  constructor() {
    this.uploadDir = path.join(__dirname, '../../uploads');
  }

  /**
   * Get upload directory path
   */
  getUploadDir(): string {
    return this.uploadDir;
  }

  /**
   * Ensure upload directory exists
   */
  async ensureUploadDir(): Promise<void> {
    try {
      await fs.access(this.uploadDir);
    } catch {
      await fs.mkdir(this.uploadDir, { recursive: true });
    }
  }

  /**
   * Check if file exists
   */
  async fileExists(filePath: string): Promise<boolean> {
    try {
      await fs.access(filePath);
      return true;
    } catch {
      return false;
    }
  }

  /**
   * Delete a file
   */
  async deleteFile(filePath: string): Promise<void> {
    try {
      await fs.unlink(filePath);
    } catch (error) {
      console.error('Error deleting file:', error);
    }
  }

  /**
   * Get file info
   */
  async getFileInfo(filePath: string): Promise<{ size: number } | null> {
    try {
      const stats = await fs.stat(filePath);
      return { size: stats.size };
    } catch {
      return null;
    }
  }
}
