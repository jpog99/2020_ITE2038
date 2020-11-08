SELECT hometown, AVG(level)
FROM CatchedPokemon INNER JOIN Trainer as t ON owner_id=t.id
GROUP BY hometown ORDER BY AVG(level);