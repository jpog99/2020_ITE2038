SELECT type, count(type)
FROM Pokemon 
WHERE type IN ('water','electric','psychic') GROUP BY type;