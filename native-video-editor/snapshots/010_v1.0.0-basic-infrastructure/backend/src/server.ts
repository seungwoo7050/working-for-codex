import express from 'express';
import cors from 'cors';
import uploadRoutes from './routes/upload.routes.js';
import { StorageService } from './services/storage.service.js';

const app = express();
const PORT = process.env.PORT || 3001;

const storageService = new StorageService();

// Middleware
app.use(cors());
app.use(express.json());

// Static file serving for uploaded videos
app.use('/videos', express.static(storageService.getUploadDir()));

// Routes
app.get('/health', (_req, res) => {
  res.json({
    status: 'ok',
    version: '1.0.0',
    service: 'video-editor-backend',
  });
});

app.use('/api', uploadRoutes);

// Error handling
app.use((err: Error, _req: express.Request, res: express.Response, _next: express.NextFunction) => {
  console.error('Error:', err.message);
  res.status(500).json({ error: err.message });
});

// Start server
app.listen(PORT, () => {
  console.log(`🚀 Video Editor Backend v1.0.0 running on http://localhost:${PORT}`);
  console.log(`📍 Health check: http://localhost:${PORT}/health`);
  console.log(`📁 Uploads directory: ${storageService.getUploadDir()}`);
});
