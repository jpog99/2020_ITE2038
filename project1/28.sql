SELECT name,AVG(level)
FROM CatchedPokemon INNER JOIN Trainer AS t ON owner_id=t.id
WHERE pid IN (SELECT id FROM Pokemon WHERE type IN('normal','electric'))
GROUP BY name
ORDER BY AVG(level);