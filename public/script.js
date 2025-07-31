// View counter
document.addEventListener('DOMContentLoaded', () => {
    let count = localStorage.getItem('pageViewCount') || 0;
    count++;
    localStorage.setItem('pageViewCount', count);
    document.getElementById('viewCount').textContent = count;

    // Background color changer
    const colors = [
        'linear-gradient(135deg, #f5f7fa 0%, #c3cfe2 100%)',
        'linear-gradient(135deg, #fff1eb 0%, #ace0f9 100%)',
        'linear-gradient(135deg, #fdfcfb 0%, #e2d1c3 100%)',
        'linear-gradient(135deg, #fad0c4 0%, #ffd1ff 100%)'
    ];
    const btn = document.getElementById('colorBtn');
    
    btn.addEventListener('click', () => {
        const randomColor = colors[Math.floor(Math.random() * colors.length)];
        document.body.style.background = randomColor;
        
        // Button animation
        btn.style.transform = 'scale(0.95)';
        setTimeout(() => {
            btn.style.transform = 'scale(1)';
        }, 100);
    });
});