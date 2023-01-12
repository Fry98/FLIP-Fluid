const CellType = {
  AIR: 0,
  LIQUID: 1,
  SOLID: 2
};

/** @type {HTMLCanvasElement} */
const canv = document.getElementById('canv');
const ctx = canv.getContext('2d');
let parts = [];

const CANV_SIZE = 800;
const GRID_SIZE = 30;
const CELL_SIZE = CANV_SIZE / GRID_SIZE;
const GRAVITY = 8;
const FLIP_RATIO = 0.85;
const MAX_DENSITY = 7;
const JACOBI_ITERS = 50;

const velXSumGrid = createNewGrid(GRID_SIZE + 1, 0);
const velYSumGrid = createNewGrid(GRID_SIZE + 1, 0);
const weightScalarGrid = createNewGrid(GRID_SIZE + 1, 0);
const weightVelXSumGrid = createNewGrid(GRID_SIZE + 1, 0);
const weightVelYSumGrid = createNewGrid(GRID_SIZE + 1, 0);
const cellTypeGrid = createNewGrid(GRID_SIZE, CellType.AIR);
const divergenceGrid = createNewGrid(GRID_SIZE, 0);
let velocityGridFrontX = createNewGrid(GRID_SIZE + 1, 0);
let velocityGridFrontY = createNewGrid(GRID_SIZE + 1, 0);
let velocityGridBackX = createNewGrid(GRID_SIZE + 1, 0);
let velocityGridBackY = createNewGrid(GRID_SIZE + 1, 0);
let pressureGridFront = createNewGrid(GRID_SIZE, 0);
let pressureGridBack = createNewGrid(GRID_SIZE, 0);

for (let x = 0; x < 15; x++) {
  cellTypeGrid[x][GRID_SIZE - 3] = CellType.SOLID;
  cellTypeGrid[x][GRID_SIZE - 2] = CellType.SOLID;
  cellTypeGrid[x][GRID_SIZE - 1] = CellType.SOLID;
}

for (let x = 0; x < 13; x++) {
  cellTypeGrid[x][GRID_SIZE - 5] = CellType.SOLID;
  cellTypeGrid[x][GRID_SIZE - 4] = CellType.SOLID;
}

for (let x = 0; x < 9; x++) {
  cellTypeGrid[x][GRID_SIZE - 6] = CellType.SOLID;
}

requestAnimationFrame(update);

function update() {
  for (let i = 0; i < MAX_DENSITY; i++) {
    parts.push([
      2 * CELL_SIZE + Math.random() * CELL_SIZE,
      1 * CELL_SIZE + Math.random() * CELL_SIZE,
      0, 0
    ]);
  }

  ctx.clearRect(0, 0, canv.width, canv.height);

  ctx.strokeStyle = '#d4d4d4';
  ctx.lineWidth = 1;
  for (let i = 0; i <= GRID_SIZE; i++) {
    ctx.moveTo(i * CELL_SIZE - CELL_SIZE * 0.5, 0);
    ctx.lineTo(i * CELL_SIZE - CELL_SIZE * 0.5, CANV_SIZE);
    ctx.stroke();

    ctx.moveTo(0, i * CELL_SIZE - CELL_SIZE * 0.5);
    ctx.lineTo(CANV_SIZE, i * CELL_SIZE - CELL_SIZE * 0.5);
    ctx.stroke();
  }

  // Transfer velocity to grid
  for (let y = 0; y <= GRID_SIZE; y++) {
    for (let x = 0; x <= GRID_SIZE; x++) {
      velXSumGrid[x][y] = 0;
      velYSumGrid[x][y] = 0;
      weightScalarGrid[x][y] = 0;
      weightVelXSumGrid[x][y] = 0;
      weightVelYSumGrid[x][y] = 0;
    }
  }

  for (const [pX, pY, velX, velY] of parts) {
    const gridX = pX / CELL_SIZE;
    const gridY = pY / CELL_SIZE;

    addToValueInterpolated(velXSumGrid, gridX + 0.5, gridY, velX);
    addToValueInterpolated(velYSumGrid, gridX, gridY + 0.5, velY);
    addToValueInterpolated(weightScalarGrid, gridX, gridY, 1.0);
    addToValueInterpolated(weightVelXSumGrid, gridX + 0.5, gridY, 1.0);
    addToValueInterpolated(weightVelYSumGrid, gridX, gridY + 0.5, 1.0);
  }

  for (let y = 0; y <= GRID_SIZE; y++) {
    for (let x = 0; x <= GRID_SIZE; x++) {
      velocityGridBackX[x][y] = weightVelXSumGrid[x][y] > 0 ? velXSumGrid[x][y] / weightVelXSumGrid[x][y] : 0;
      velocityGridBackY[x][y] = weightVelYSumGrid[x][y] > 0 ? velYSumGrid[x][y] / weightVelYSumGrid[x][y] : 0;
      velocityGridFrontX[x][y] = weightVelXSumGrid[x][y] > 0 ? velXSumGrid[x][y] / weightVelXSumGrid[x][y] : 0;
      velocityGridFrontY[x][y] = weightVelYSumGrid[x][y] > 0 ? velYSumGrid[x][y] / weightVelYSumGrid[x][y] : 0;
    }
  }

  // Add forces
  for (let y = 0; y <= GRID_SIZE; y++) {
    for (let x = 0; x <= GRID_SIZE; x++) {
      velocityGridFrontY[x][y] += GRAVITY;
    }
  }

  enforceBounds();

  // Divergence
  for (let y = 0; y < GRID_SIZE; y++) {
    for (let x = 0; x < GRID_SIZE; x++) {
      if (getValueInterpolated(weightScalarGrid, x, y, GRID_SIZE + 1) === 0) continue;

      const leftX = getValueInterpolated(velocityGridFrontX, x, y, GRID_SIZE + 1);
      const rightX = getValueInterpolated(velocityGridFrontX, x + 1, y, GRID_SIZE + 1);
      const bottomY = getValueInterpolated(velocityGridFrontY, x, y, GRID_SIZE + 1);
      const topY = getValueInterpolated(velocityGridFrontY, x, y + 1, GRID_SIZE + 1);

      const divergence = (rightX - leftX) + (topY - bottomY);
      const density = getValueInterpolated(weightScalarGrid, x, y, GRID_SIZE + 1);
      divergenceGrid[x][y] = divergence - Math.max((density - MAX_DENSITY), 0.0);
    }
  }

  // Jacobi
  for (let y = 0; y < GRID_SIZE; y++) {
    for (let x = 0; x < GRID_SIZE; x++) {
      pressureGridFront[x][y] = 0;
      pressureGridBack[x][y] = 0;
    }
  }

  for (let i = 0; i < JACOBI_ITERS; i++) {
    for (let y = 0; y < GRID_SIZE; y++) {
      for (let x = 0; x < GRID_SIZE; x++) {
        if (getValueInterpolated(weightScalarGrid, x, y, GRID_SIZE + 1) === 0) continue;
        const divergence = divergenceGrid[x][y];

        const left = pressureGridBack[clampToGrid(x - 1)][y];
        const right = pressureGridBack[clampToGrid(x + 1)][y];
        const bottom = pressureGridBack[x][clampToGrid(y - 1)];
        const top = pressureGridBack[x][clampToGrid(y + 1)];

        const pressure = (left + right + top + bottom - divergence) / 4;
        pressureGridFront[x][y] = pressure;
      }
    }

    const temp = pressureGridBack;
    pressureGridBack = pressureGridFront;
    pressureGridFront = temp;
  }

  // Subtract gradient from velocity
  for (let y = 0; y <= GRID_SIZE; y++) {
    for (let x = 0; x <= GRID_SIZE; x++) {
      const left = getValueInterpolated(pressureGridBack, x - 1, y, GRID_SIZE);
      const right = getValueInterpolated(pressureGridBack, x, y, GRID_SIZE);
      const bottom = getValueInterpolated(pressureGridBack, x, y - 1, GRID_SIZE);
      const top = getValueInterpolated(pressureGridBack, x, y, GRID_SIZE);

      const gradX = right - left;
      const gradY = top - bottom;
      velocityGridFrontX[x][y] -= gradX;
      velocityGridFrontY[x][y] -= gradY;
    }
  }

  enforceBounds();

  // Transfer velocity to particles
  for (let i = 0; i < parts.length; i++) {
    const [pX, pY, velX, velY] = parts[i];
    const gridX = pX / CELL_SIZE;
    const gridY = pY / CELL_SIZE;

    const ogVelX = getValueInterpolated(velocityGridBackX, gridX + 0.5, gridY, GRID_SIZE + 1);
    const ogVelY = getValueInterpolated(velocityGridBackY, gridX, gridY + 0.5, GRID_SIZE + 1);
    const currVelX = getValueInterpolated(velocityGridFrontX, gridX + 0.5, gridY, GRID_SIZE + 1);
    const currVelY = getValueInterpolated(velocityGridFrontY, gridX, gridY + 0.5, GRID_SIZE + 1);

    const velDeltaX = currVelX - ogVelX;
    const velDeltaY = currVelY - ogVelY;
    const flipVelX = velX + velDeltaX;
    const flipVelY = velY + velDeltaY;

    parts[i][2] = flipVelX * FLIP_RATIO + currVelX * (1 - FLIP_RATIO);
    parts[i][3] = flipVelY * FLIP_RATIO + currVelY * (1 - FLIP_RATIO);
  }

  // Advect particles
  for (let i = 0; i < parts.length; i++) {
    const gridX = parts[i][0] / CELL_SIZE;
    const gridY = parts[i][1] / CELL_SIZE;
    const originVelX = getValueInterpolated(velocityGridFrontX, gridX + 0.5, gridY, GRID_SIZE + 1);
    const originVelY = getValueInterpolated(velocityGridFrontY, gridX, gridY + 0.5, GRID_SIZE + 1);

    const halfPosX = (parts[i][0] + originVelX * 0.5) / CELL_SIZE;
    const halfPosY = (parts[i][1] + originVelY * 0.5) / CELL_SIZE;
    const halfVelX = getValueInterpolated(velocityGridFrontX, halfPosX + 0.5, halfPosY, GRID_SIZE + 1);
    const halfVelY = getValueInterpolated(velocityGridFrontY, halfPosX, halfPosY + 0.5, GRID_SIZE + 1);

    const newPosX = parts[i][0] + halfVelX;
    const newPosY = parts[i][1] + halfVelY;

    if (newPosX >= 0 && newPosX <= CANV_SIZE && newPosY >= 0 && newPosY <= CANV_SIZE) {
      parts[i][0] = newPosX;
      parts[i][1] = newPosY;
    }
  }

  ctx.fillStyle = '#fcddc0';
  for (let y = 0; y < GRID_SIZE; y++) {
    for (let x = 0; x < GRID_SIZE; x++) {
      if (cellTypeGrid[x][y] === CellType.SOLID) {
        ctx.fillRect(x * CELL_SIZE + 1 - CELL_SIZE * 0.5, y * CELL_SIZE + 1 - CELL_SIZE * 0.5, CELL_SIZE - 2, CELL_SIZE - 2);
      }
    }
  }

  // Draw particles
  ctx.fillStyle = 'blue';
  for (const [px, py] of parts) {
    ctx.beginPath();
    ctx.arc(px, py, 3, 0, Math.PI * 2);
    ctx.fill();
  }

  if (parts.length > 5_000) {
    parts.splice(0, 3_500);
  }

  requestAnimationFrame(update);
}

function enforceBounds() {
  for (let x = 0; x <= GRID_SIZE; x++) {
    velocityGridFrontY[x][0] = 0;
    velocityGridFrontX[x][GRID_SIZE] = 0;
    velocityGridFrontY[x][GRID_SIZE] = 0;
  }

  for (let y = 0; y <= GRID_SIZE; y++) {
    velocityGridFrontX[0][y] = 0;
    velocityGridFrontX[GRID_SIZE][y] = 0;
    velocityGridFrontY[GRID_SIZE][y] = 0;
  }

  for (let y = 0; y < GRID_SIZE; y++) {
    for (let x = 0; x < GRID_SIZE; x++) {
      const center = cellTypeGrid[clampToGrid(x)][clampToGrid(y)];
      const top = cellTypeGrid[clampToGrid(x)][clampToGrid(y - 1)];
      const left = cellTypeGrid[clampToGrid(x - 1)][clampToGrid(y)];

      if (center === CellType.SOLID || left === CellType.SOLID) {
        velocityGridFrontX[x][y] = 0;
      }

      if (center === CellType.SOLID || top === CellType.SOLID) {
        velocityGridFrontY[x][y] = 0;
      }
    }
  }
}

function createNewGrid(size, val) {
  return typeof valGen === 'function' ?
    [...Array(size)].map(() => [...Array(size)].map(val)) :
    [...Array(size)].map(() => Array(size).fill(val));
}

function clampToGrid(val) {
  return clamp(val, 0, GRID_SIZE - 1);
}

function clamp(val, min, max) {
  return Math.min(Math.max(val, min), max);
}

function getValueInterpolated(grid, x, y, gridSize) {
  const xFrac = x % 1;
  const yFrac = y % 1;

  x = clamp(Math.floor(x), 0, gridSize - 1);
  y = clamp(Math.floor(y), 0, gridSize - 1);
  const xPlus1 = clamp(x + 1, 0, gridSize - 1);
  const yPlus1 = clamp(y + 1, 0, gridSize - 1);

  const val00 = grid[x][y];
  const val10 = grid[xPlus1][y];
  const val01 = grid[x][yPlus1];
  const val11 = grid[xPlus1][yPlus1];

  const val0 = (1 - xFrac) * val00 + xFrac * val10;
  const val1 = (1 - xFrac) * val01 + xFrac * val11;
  return (1 - yFrac) * val0 + yFrac * val1;
}

function addToValueInterpolated(grid, x, y, value) {
  const xFrac = x % 1;
  const yFrac = y % 1;

  x = clamp(Math.floor(x), 0, GRID_SIZE);
  y = clamp(Math.floor(y), 0, GRID_SIZE);
  const xPlus1 = clamp(x + 1, 0, GRID_SIZE);
  const yPlus1 = clamp(y + 1, 0, GRID_SIZE);

  const int0 = (1 - yFrac) * value;
  const int1 = yFrac * value;

  const int00 = (1 - xFrac) * int0;
  const int10 = xFrac * int0;
  const int01 = (1 - xFrac) * int1;
  const int11 = xFrac * int1;

  grid[x][y] += int00;
  grid[xPlus1][y] += int10;
  grid[x][yPlus1] += int01;
  grid[xPlus1][yPlus1] += int11;
}
