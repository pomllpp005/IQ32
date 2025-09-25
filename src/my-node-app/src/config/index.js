module.exports = {
    db: {
        host: process.env.DB_HOST || 'localhost',
        port: process.env.DB_PORT || 27017,
        name: process.env.DB_NAME || 'mydatabase',
    },
    server: {
        port: process.env.PORT || 3000,
    },
    jwtSecret: process.env.JWT_SECRET || 'your_jwt_secret',
    environment: process.env.NODE_ENV || 'development',
};